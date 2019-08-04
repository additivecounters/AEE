#include <iostream>
#include <stdlib.h>
#include <random>
#include <chrono>
#include <math.h>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "CountMinAEE.hpp"

using namespace std;


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


inline unsigned CountMinAEE::aaeeelerated_geometric(double LogOneMinusP) {
	// Return a randomly drawn geometric variable with parameter p, such that LogOneMinusP = log(1-p)
	return log(dis_arr(generator_arr)) / LogOneMinusP + 1;
}

CountMinAEE::CountMinAEE(int width, int height, int seed) :

	gen_arr(seed),
	dis_arr(0, 1),
	rand_bits(_mm256_set_epi64x(UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX)),
	RandByteArray((char *)&rand_bits),
	weighted_rand_bits(_mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr())),
	WeightedRandByteArray((uint16_t *)&weighted_rand_bits)
{

	this->seed = seed;
	this->width = width;
	this->height = height;

	width_mask = width - 1;

	assert(width > 0 && "We assume too much!");
	assert(width % 4 == 0 && "We assume that (w % 4 == 0)!");
	assert((width & (width - 1)) == 0 && "We assume that width is a power of 2!");

	aee_cms = new aee_counter_t*[height];
	for (int i = 0; i < height; ++i)
	{
		aee_cms[i] = new aee_counter_t[width]();
	}

	bobhash = new BOBHash[height];
	for (int i = 0; i < height; ++i)
	{
		bobhash[i].initialize(i + 1000);
	}

	generator_arr.seed(seed);
	LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p_bound));
	interval = aaeeelerated_geometric(LogOneMinusP);

	assert(width % 4 == 0 && "We assume that (w % 4 == 0)!");

	width_over_four = width >> 2;

	indices = new int[height];
}

CountMinAEE::~CountMinAEE()
{
	for (int i = 0; i < height; ++i)
	{
		delete[] aee_cms[i];
	}

	delete[] aee_cms;
	delete[] bobhash;
	delete[] indices;
}

void CountMinAEE::divide_array_counters()
{
	for (int i = 0; i < height; ++i)
	{
		uint64_t* four_counters_array = (uint64_t*)aee_cms[i];
		for (unsigned j = 0; j < width_over_four; ++j)
		{
			uint64_t &four_counters = four_counters_array[j];
			four_counters = (four_counters & division_mask) >> 1;
		}
	}
}

void CountMinAEE::increment(const char * str)
{
	if (minus_log_p == 0)
	{
		increment_h(str);
	}
	else if (minus_log_p < minus_log_p_bound)
	{
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

		if (RandByteArray[rand_pos >> 3] & (1 << ((rand_pos & 0x7) - 1))) // TODO: lookup table 
		{
			increment_h(str);
		}
		else
		{
		}
	}
	else
	{
		if (--interval)
		{
		}
		else
		{
			increment_h(str);
			interval = aaeeelerated_geometric(LogOneMinusP);
		}
	}
}

void CountMinAEE::increment_h(const char * str)
{
	// approximation of the correct logic - as we may divide after going only over a fraction of the counters.
	for (int i = 0; i < height; ++i) 
	{
		aee_counter_t &comp_counter = aee_cms[i][bobhash[i].run(str, FT_SIZE) % width];
		if (comp_counter == 65535)
		{
			++minus_log_p;
			LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p));
			divide_array_counters();

			// if p decreases we need to reduce sampling rate aaeeordingly.
			rand_bits = _mm256_and_si256(rand_bits, _mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr()));
		}
		comp_counter += 1; 
	}
}

void CountMinAEE::add(const char * str, int c)
{
	if (minus_log_p == 0)
	{
		add_h(str, c);
		return;
	}

	else if (minus_log_p < minus_log_p_bound)
	{

		if (weighted_rand_pos < 16)
		{
		}
		else
		{
			weighted_rand_bits = _mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr());
			WeightedRandByteArray = (uint16_t *)&weighted_rand_bits;
			weighted_rand_pos = 0;
		}

		w1 = c >> minus_log_p;
		w2 = c - (w1 << minus_log_p);

		uint32_t increment = w1;
		
		if (WeightedRandByteArray[weighted_rand_pos] <= w2 << (16 - minus_log_p))
		{
			increment += 1;
		}
		++weighted_rand_pos;

		add_h(str, increment);
	}

	else
	{		
		if (minus_log_p >= 15) 
		{
			uint32_t increment = 0;
			while (w >= interval)
			{
				++increment;
				w -= interval;
				interval = aaeeelerated_geometric(LogOneMinusP);
			}
			interval -= w;

			if (increment == 0)
			{
			}
			else
			{
				add_h(str, increment);
			}	
		}		
		else
		{
			w1 = c >> minus_log_p;
			w2 = c - (w1 << minus_log_p);

			uint32_t increment = w1;

			while (w2 >= interval)
			{
				++increment;
				w2 -= interval;
				interval = aaeeelerated_geometric(LogOneMinusP);
			}
			interval -= w2;

			if (increment == 0)
			{
			}
			else
			{
				add_h(str, increment);
			}
		}		
	}
}

void CountMinAEE::add_h(const char * str, int c)
{
	// approximation of the correct logic - as we may divide after going only over a fraction of the counters.
	indices[0] = bobhash[0].run(str, FT_SIZE) % width;
	aee_counter_t max = aee_cms[0][indices[0]];
	for (int i = 1; i < height; ++i)
	{
		indices[i] = bobhash[i].run(str, FT_SIZE) % width;
		if (max < aee_cms[i][indices[i]])
		{
			max = aee_cms[i][indices[i]];
		}
	}

	if (max > 65535 - c)
	{
		// TODO: devide p several times?
		++minus_log_p;
		LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p));
		interval = aaeeelerated_geometric(LogOneMinusP);

		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				if (j != indices[i]) 
				{
					aee_cms[i][j] /= 2;
				}
				else
				{
					curr_sum = aee_cms[i][j] + c;
					aee_cms[i][j] = curr_sum / 2;
				}
			}
		}
		return;
	}
	else
	{
		for (int i = 0; i < height; ++i)
		{
			aee_cms[i][indices[i]] += c;
		}
	}
}

uint64_t CountMinAEE::query(const char * str)
{
	// approximation of the correct logic - as we may divide after going only over a fraction of the counters.
	aee_counter_t min = aee_cms[0][bobhash[0].run(str, FT_SIZE) % width];
	for (int i = 1; i < height; ++i)
	{
		aee_counter_t &comp_counter = aee_cms[i][bobhash[i].run(str, FT_SIZE) % width];
		if (min > comp_counter)
		{
			min = comp_counter;
		}
	}
	return ((uint64_t)min) << minus_log_p;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

inline unsigned ConservativeUpdateAEE::aaeeelerated_geometric(double LogOneMinusP) {
	// Return a randomly drawn geometric variable with parameter p, such that LogOneMinusP = log(1-p)
	return log(dis_arr(generator_arr)) / LogOneMinusP + 1;
}

ConservativeUpdateAEE::ConservativeUpdateAEE(int width, int height, int seed) :

	gen_arr(seed),
	dis_arr(0, 1),
	rand_bits(_mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr())),
	RandByteArray((char *)&rand_bits)

{

	this->seed = seed;
	this->width = width;
	this->height = height;

	width_mask = width - 1;

	assert(width > 0 && "We assume too much!");
	assert(width % 4 == 0 && "We assume that (w % 4 == 0)!");
	assert((width & (width - 1)) == 0 && "We assume that width is a power of 2!");

	aee_cus = new aee_counter_t*[height];
	for (int i = 0; i < height; ++i)
	{
		aee_cus[i] = new aee_counter_t[width]();
	}

	bobhash = new BOBHash[height];
	for (int i = 0; i < height; ++i)
	{
		bobhash[i].initialize(i + 1000);
	}

	generator_arr.seed(seed);
	LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p_bound));
	interval = aaeeelerated_geometric(LogOneMinusP);

	assert(width % 4 == 0 && "We assume that (w % 4 == 0)!");

	width_over_four = width >> 2;

	indices = new int[height];
}

ConservativeUpdateAEE::~ConservativeUpdateAEE()
{
	delete[] indices;
	delete[] bobhash;
	
	for (int i = 0; i < height; ++i)
	{
		delete[] aee_cus[i];
	}
	delete[] aee_cus;	
}

void ConservativeUpdateAEE::divide_array_counters()
{
	for (int i = 0; i < height; ++i)
	{
		uint64_t* four_counters_array = (uint64_t*)aee_cus[i];
		for (unsigned j = 0; j < width_over_four; ++j)
		{
			uint64_t &four_counters = four_counters_array[j];
			four_counters = (four_counters & division_mask) >> 1;
		}
	}
}

void ConservativeUpdateAEE::increment(const char * str)
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
		if (--interval)
		{
		}
		else
		{
			increment_h(str);
			interval = aaeeelerated_geometric(LogOneMinusP);
		}
	}
}

void ConservativeUpdateAEE::increment_h(const char * str)
{
	// approximation of the correct logic - as we may divide after going only over a fraction of the counters.
	indices[0] = bobhash[0].run(str, FT_SIZE) % width;
	aee_counter_t min = aee_cus[0][indices[0]];
	for (int i = 1; i < height; ++i)
	{
		indices[i] = bobhash[i].run(str, FT_SIZE) % width;
		if (min > aee_cus[i][indices[i]])
		{
			min = aee_cus[i][indices[i]];
		}
	}

	if (min == 65535)
	{
		++minus_log_p;
		LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p));
		divide_array_counters();
		min = 32767;
	}

	for (int i = 0; i < height; ++i)
	{
		aee_counter_t &comp_counter = aee_cus[i][indices[i]];
		if (comp_counter == min)
		{
			++comp_counter;
		}		
	}
}

uint64_t ConservativeUpdateAEE::query(const char * str)
{
	// approximation of the correct logic - as we may divide after going only over a fraction of the counters.
	aee_counter_t min = aee_cus[0][bobhash[0].run(str, FT_SIZE) % width];
	for (int i = 1; i < height; ++i)
	{
		aee_counter_t &comp_counter = aee_cus[i][bobhash[i].run(str, FT_SIZE) % width];
		if (min > comp_counter)
		{
			min = comp_counter;
		}
	}
	return ((uint64_t)min) << minus_log_p;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

// Baseline - for speed and error comparison
CountMinBaseline::CountMinBaseline()
{
}

CountMinBaseline::~CountMinBaseline()
{
	for (int i = 0; i < height; ++i)
	{
		delete[] baseline_cms[i];
	}
	delete[] bobhash;
	delete[] baseline_cms;
}

void CountMinBaseline::initialize(int width, int height)
{
	this->width = width;
	this->height = height;

	width_mask = width - 1;

	assert(width > 0 && "We assume too much!");
	assert(width % 4 == 0 && "We assume that (w % 4 == 0)!");
	assert((width & (width - 1)) == 0 && "We assume that width is a power of 2!");

	baseline_cms = new uint32_t*[height];
	bobhash = new BOBHash[height];

	for (int i = 0; i < height; ++i)
	{
		baseline_cms[i] = new uint32_t[width]();
		bobhash[i].initialize(i + 1000);
	}
}

void CountMinBaseline::increment(const char * str)
{
	for (int i = 0; i < height; ++i) {
		uint index = (bobhash[i].run(str, FT_SIZE)) & width_mask;
		++baseline_cms[i][index];
	}
}


uint64_t CountMinBaseline::query(const char * str)
{
	uint index = (bobhash[0].run(str, FT_SIZE)) & width_mask;
	uint64_t min = baseline_cms[0][index];
	for (int i = 1; i < height; ++i) {
		uint index = (bobhash[i].run(str, FT_SIZE)) & width_mask;
		uint64_t temp = baseline_cms[i][index];
		if (min > temp)
		{
			min = temp;
		}
	}
	return min;
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

// Weighted Baseline - for speed and error comparison
WeightedCountMinBaseline::WeightedCountMinBaseline()
{
}

WeightedCountMinBaseline::~WeightedCountMinBaseline()
{
	for (int i = 0; i < height; ++i)
	{
		delete[] baseline_cms[i];
	}
	delete[] bobhash;
	delete[] baseline_cms;
}

void WeightedCountMinBaseline::initialize(int width, int height)
{
	this->width = width;
	this->height = height;

	width_mask = width - 1;

	assert(width > 0 && "We assume too much!");
	assert(width % 4 == 0 && "We assume that (w % 4 == 0)!");
	assert((width & (width - 1)) == 0 && "We assume that width is a power of 2!");

	baseline_cms = new uint64_t*[height];
	bobhash = new BOBHash[height];

	for (int i = 0; i < height; ++i)
	{
		baseline_cms[i] = new uint64_t[width]();
		bobhash[i].initialize(i + 1000);
	}
}

void WeightedCountMinBaseline::add(const char * str, int c)
{
	for (int i = 0; i < height; ++i) {
		uint index = (bobhash[i].run(str, FT_SIZE)) & width_mask;
		baseline_cms[i][index] += c;
	}
}


uint64_t WeightedCountMinBaseline::query(const char * str)
{
	uint index = (bobhash[0].run(str, FT_SIZE)) & width_mask;
	uint64_t min = baseline_cms[0][index];
	for (int i = 1; i < height; ++i) {
		uint index = (bobhash[i].run(str, FT_SIZE)) & width_mask;
		uint64_t temp = baseline_cms[i][index];
		if (min > temp)
		{
			min = temp;
		}
	}
	return min;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


ConservativeUpdateBaseline::ConservativeUpdateBaseline()
{
}

ConservativeUpdateBaseline::~ConservativeUpdateBaseline()
{
	for (int i = 0; i < height; ++i)
	{
		delete[] baseline_cu[i];
	}
	delete[] bobhash;
	delete[] baseline_cu;

	delete[] counters;
}

void ConservativeUpdateBaseline::initialize(int width, int height)
{
	this->width = width;
	this->height = height;

	width_mask = width - 1;

	assert(width > 0 && "We assume too much!");
	assert(width % 4 == 0 && "We assume that (w % 4 == 0)!");
	assert((width & (width - 1)) == 0 && "We assume that width is a power of 2!");

	baseline_cu = new uint32_t*[height];
	bobhash = new BOBHash[height];

	for (int i = 0; i < height; ++i)
	{
		baseline_cu[i] = new uint32_t[width]();
		bobhash[i].initialize(i + 1000);
	}

	counters = new uint32_t*[height];
}

void ConservativeUpdateBaseline::increment(const char * str)
{
	uint index = (bobhash[0].run(str, FT_SIZE)) & width_mask;
	counters[0] = baseline_cu[0] + index;
	uint64_t min = *counters[0];
	
	for (int i = 1; i < height; ++i) {
		uint index = (bobhash[i].run(str, FT_SIZE)) & width_mask;
		counters[i] = baseline_cu[i] + index;
		uint64_t temp = *counters[i];
		if (min > temp)
		{
			min = temp;
		}
	}

	for (int i = 0; i < height; ++i) {
		if (*counters[i] == min)
		{
			++(*counters[i]);
		}	
	}
}

uint64_t ConservativeUpdateBaseline::query(const char * str)
{
	uint index = (bobhash[0].run(str, FT_SIZE)) & width_mask;
	uint64_t min = baseline_cu[0][index];
	for (int i = 1; i < height; ++i) {
		uint index = (bobhash[i].run(str, FT_SIZE)) & width_mask;
		uint64_t temp = baseline_cu[i][index];
		if (min > temp)
		{
			min = temp;
		}
	}
	return min;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

inline unsigned CountMinAEE_MaxSpeed::aaeeelerated_geometric(double LogOneMinusP) {
	// Return a randomly drawn geometric variable with parameter p, such that LogOneMinusP = log(1-p)
	return log(dis_arr(generator_arr)) / LogOneMinusP + 1;
}

CountMinAEE_MaxSpeed::CountMinAEE_MaxSpeed(int width, int height, int seed, double epsilon_aee, double delta_aee) :

	gen_arr(seed),
	dis_arr(0, 1),
	rand_bits(_mm256_set_epi64x(UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX)),
	RandByteArray((char *)&rand_bits),
	weighted_rand_bits(_mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr())),
	WeightedRandByteArray((uint16_t *)&weighted_rand_bits)
{

	this->seed = seed;
	this->width = width;
	this->height = height;

	width_mask = width - 1;

	assert(width > 0 && "We assume too much!");
	assert(width % 4 == 0 && "We assume that (w % 4 == 0)!");
	assert((width & (width - 1)) == 0 && "We assume that width is a power of 2!");

	aee_cms = new aee_counter_t*[height];
	for (int i = 0; i < height; ++i)
	{
		aee_cms[i] = new aee_counter_t[width]();
	}

	bobhash = new BOBHash[height];
	for (int i = 0; i < height; ++i)
	{
		bobhash[i].initialize(i + 1000);
	}

	generator_arr.seed(seed);
	LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p_bound));
	interval = aaeeelerated_geometric(LogOneMinusP);

	assert(width % 4 == 0 && "We assume that (w % 4 == 0)!");

	width_over_four = width >> 2;

	indices = new int[height];

	N_current = 0;
	N_prime = 2 * (1 + epsilon_aee / 3)*(1 / (epsilon_aee*epsilon_aee))*log(2 / delta_aee) + 1;
	Shifted_N_prime = N_prime;

	assert((N_prime <= 1 << 16) && "N_prime is too large! For this (eplison, delta) we need more than 16 bits!");
}

CountMinAEE_MaxSpeed::~CountMinAEE_MaxSpeed()
{
	for (int i = 0; i < height; ++i)
	{
		delete[] aee_cms[i];
	}

	delete[] aee_cms;
	delete[] bobhash;
	delete[] indices;
}

void CountMinAEE_MaxSpeed::divide_array_counters()
{
	for (int i = 0; i < height; ++i)
	{
		uint64_t* four_counters_array = (uint64_t*)aee_cms[i];
		for (unsigned j = 0; j < width_over_four; ++j)
		{
			uint64_t &four_counters = four_counters_array[j];
			four_counters = (four_counters & division_mask) >> 1;
		}
	}
}

void CountMinAEE_MaxSpeed::increment(const char * str)
{
	++N_current;

	if (minus_log_p == 0)
	{
		increment_h(str);
	}
	else if (minus_log_p < minus_log_p_bound)
	{
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

		if (RandByteArray[rand_pos >> 3] & (1 << ((rand_pos & 0x7) - 1))) // TODO: lookup table 
		{
			increment_h(str);
		}
		else
		{
		}
	}
	else
	{
		if (--interval)
		{
		}
		else
		{
			increment_h(str);
			interval = aaeeelerated_geometric(LogOneMinusP);
		}
	}
}

void CountMinAEE_MaxSpeed::increment_h(const char * str)
{

	if (N_current > Shifted_N_prime)
	{
		Shifted_N_prime *= 2;
		++minus_log_p;
		LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p));
		divide_array_counters();

		// if p decreases we need to reduce sampling rate aaeeordingly.
		rand_bits = _mm256_and_si256(rand_bits, _mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr()));
	}

	// approximation of the correct logic - as we may divide after going only over a fraction of the counters.
	for (int i = 0; i < height; ++i)
	{
		aee_counter_t &comp_counter = aee_cms[i][bobhash[i].run(str, FT_SIZE) % width];
		++comp_counter;

		assert((comp_counter <= 65535) && "This cannot happen! MaxSpeed is Bob!");
	}
}

uint64_t CountMinAEE_MaxSpeed::query(const char * str)
{
	// approximation of the correct logic - as we may divide after going only over a fraction of the counters.
	aee_counter_t min = aee_cms[0][bobhash[0].run(str, FT_SIZE) % width];
	for (int i = 1; i < height; ++i)
	{
		aee_counter_t &comp_counter = aee_cms[i][bobhash[i].run(str, FT_SIZE) % width];
		if (min > comp_counter)
		{
			min = comp_counter;
		}
	}
	return ((uint64_t)min) << minus_log_p;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
