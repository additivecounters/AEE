#include <iostream>
#include <stdlib.h>
#include <random>
#include <chrono>
#include <math.h>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "DwayRapAEE.hpp"


using namespace std;


inline unsigned Dway_Rap_AEE::aaeeelerated_geometric(double LogOneMinusP) {
	// Return a randomly drawn geometric variable with parameter p, such that LogOneMinusP = log(1-p)
	return log(dis_arr(generator_arr)) / LogOneMinusP + 1;
}


Dway_Rap_AEE::Dway_Rap_AEE(int seed, int d, int num_rows) :

	gen_arr(seed),
	dis_arr(0, 1),
	rand_bits(_mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr())),
	RandByteArray((char *)&rand_bits),
	bob_way(seed),
	bob_fp(seed+2)

{
	generator_arr.seed(seed);
	LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p_bound));
	aee_interval = aaeeelerated_geometric(LogOneMinusP);
	
	assert(d % 4 == 0 && "We assume that (w % 4 == 0)!");

	d_over_four = d >> 2;

	aee_arrays = new uint16_t*[num_rows];
	aee_fingerprints = new uint32_t*[num_rows];

	for (int i = 0; i < num_rows; ++i)
	{
		aee_arrays[i] = new uint16_t[d]();
		aee_fingerprints[i] = new uint32_t[d]();
	}

	this->d = d;
	this->num_rows = num_rows;
}


void Dway_Rap_AEE::divide_array_counters()
{
	for (int i = 0; i < num_rows; ++i)
	{
		uint64_t* four_counters_array = (uint64_t*)aee_arrays[i];
		for (unsigned j = 0; j < d_over_four; ++j)
		{
			uint64_t &four_counters = four_counters_array[j];
			four_counters = (four_counters & division_mask) >> 1;
		}
	}
}

void Dway_Rap_AEE::increment_h(const char * str)
{
	uint32_t fp = bob_fp.run(str, FT_SIZE); 
	uint32_t way =bob_way.run(str, FT_SIZE) % num_rows;

	uint16_t* aee_array = aee_arrays[way];
	uint32_t* aee_fingerprint = aee_fingerprints[way];

	int index = find_fingerprint(fp, aee_fingerprint);
	
	if (index == -1)
	{
		// RAP
		index = find_min(aee_array);
		double p = 1.0/(aee_array[index]);
		
		if (gen_arr() > UINT64_MAX*p)
		{
			return;
		}
		else
		{
			aee_fingerprint[index] = fp;
		}
	}
	
	uint16_t &comp_counter = aee_arrays[way][index];

	if (comp_counter == 65535)
	{
		++minus_log_p;
		divide_array_counters();
	}
	comp_counter += 1;
	LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p));
}

void Dway_Rap_AEE::increment(const char * str)
{

	if (minus_log_p == 0)
	{
		increment_h(str);
	}
	else if (minus_log_p < minus_log_p_bound)
	{
		if (RandByteArray[rand_pos >> 3] & (1 << ((rand_pos & 0x7) - 1))) // TODO: lookup table 
		{
			increment_h(str);
		}
		else
		{
		}

		if (rand_pos < 255)
		{
			++rand_pos;
		}
		else
		{
			rand_bits = _mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr());
			for (int j = 0; j < minus_log_p - 1; ++j)
			{
				rand_bits = _mm256_and_si256(rand_bits, _mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr()));
			}
			RandByteArray = (char *)&rand_bits;
			rand_pos = 0;
		}
	}
	else
	{
		if (--aee_interval)
		{
		}
		else
		{
			increment_h(str);
			aee_interval = aaeeelerated_geometric(LogOneMinusP);
		}
	}
}


uint64_t Dway_Rap_AEE::query(const char * str)
{

	uint32_t fp = bob_fp.run(str, FT_SIZE);
	uint32_t way = bob_way.run(str, FT_SIZE) % num_rows; 

	uint16_t* aee_array = aee_arrays[way];
	uint32_t* aee_fingerprint = aee_fingerprints[way];

	int index = find_fingerprint(fp, aee_fingerprint);

	if (index == -1)
	{
		return 0;
	}

	return ((uint64_t)(aee_arrays[way][index])) << minus_log_p;
}


int Dway_Rap_AEE::find_fingerprint(uint32_t fp, uint32_t* aee_fingerprint)
{	
	for (int i = 0; i < d; ++i)
	{
		if (aee_fingerprint[i] == fp)
		{
			return i;
		}
	}
	return -1;
}


int Dway_Rap_AEE::find_min(uint16_t* aee_array)
{
	int min = aee_array[0], min_index = 0;
	
	for (int i = 1; i < d; ++i)
	{
		if (aee_array[i] < min)
		{
			min = aee_array[i];
			min_index = i;
		}
	}
	return min_index;
}
