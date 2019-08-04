#include <iostream>
#include <stdlib.h>
#include <random>
#include <chrono>
#include <math.h>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "SpaceSavingAEE.hpp"


using namespace std;


inline unsigned Space_Saving_AEE::aaeeelerated_geometric(double LogOneMinusP) {
	// Return a randomly drawn geometric variable with parameter p, such that LogOneMinusP = log(1-p)
	return log(dis_arr(generator_arr)) / LogOneMinusP + 1;
}


Space_Saving_AEE::Space_Saving_AEE(int seed, int width) :

	gen_arr(seed),
	dis_arr(0, 1),
	rand_bits(_mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr())),
	RandByteArray((char *)&rand_bits),
	bob(seed)

{
	generator_arr.seed(seed);
	LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p_bound));
	interval = aaeeelerated_geometric(LogOneMinusP);

	assert(width % 4 == 0 && "We assume that (w % 4 == 0)!");

	width_over_four = width >> 2;
	aee_array = new uint16_t[width]();
	aee_fingerprints = new uint32_t[width]();

	this->width = width;
}


void Space_Saving_AEE::divide_array_counters()
{
	uint64_t* four_counters_array = (uint64_t*)aee_array;
	for (unsigned i = 0; i < width_over_four; ++i)
	{
		uint64_t &four_counters = four_counters_array[i];
		four_counters = (four_counters & division_mask) >> 1;
	}
}


void Space_Saving_AEE::increment(const char * str)
{

	uint32_t fp = bob.run(str, strlen(str));

	int index = find_fingerprint(fp);

	if (index == -1)
	{
		index = find_min();
		aee_fingerprints[index] = fp;
	}

	if (minus_log_p == 0)
	{
		uint16_t &comp_counter = aee_array[index];

		if (comp_counter == 65535)
		{
			++minus_log_p;
			divide_array_counters();
		}
		comp_counter += 1;
	}
	else if (minus_log_p < minus_log_p_bound)
	{
		if (RandByteArray[rand_pos >> 3] & (1 << ((rand_pos & 0x7) - 1))) // TODO: lookup table 
		{
			uint16_t &comp_counter = aee_array[index];

			if (comp_counter == 65535)
			{
				++minus_log_p;
				divide_array_counters();
			}
			comp_counter += 1;
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
		if (--interval)
		{
		}
		else
		{
			uint16_t &comp_counter = aee_array[index];

			if (comp_counter == 65535)
			{
				++minus_log_p;
				LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p));
				divide_array_counters();
			}
			comp_counter += 1;
			interval = aaeeelerated_geometric(LogOneMinusP);
		}
	}
}


uint64_t Space_Saving_AEE::query(const char * str)
{

	uint32_t fp = bob.run(str, strlen(str));

	int index = find_fingerprint(fp);

	if (index == -1)
	{
		index = find_min();
	}

	return ((uint64_t)(aee_array[index])) << minus_log_p;
}


int Space_Saving_AEE::find_fingerprint(uint32_t fp)
{
	for (int i = 0; i < width; ++i)
	{
		if (aee_fingerprints[i] == fp)
		{
			return i;
		}
	}
	return -1;
}


int Space_Saving_AEE::find_min()
{
	int min = aee_array[0], min_index = 0;
	for (int i = 1; i < width; ++i)
	{
		if (aee_array[i] < min)
		{
			min = aee_array[i];
			min_index = i;
		}
	}
	return min_index;
}

