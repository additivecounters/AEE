#pragma once

#ifndef COUNT_MIN_AEE
#define COUNT_MIN_AEE

#include <iostream>
#include <stdlib.h>
#include <random>
#include <chrono>
#include <math.h>
#include <fstream>
#include <assert.h>
#include <time.h>

#include "RngFast.hpp"
#include "BobHash.hpp"
#include "Defs.hpp"

using namespace std;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

class CountMinAEE {

	int seed;

	int width;
	int height;

	int width_mask;

	BOBHash *bobhash;

	aee_counter_t **aee_cms;

	int* indices;

	mt19937 generator_arr;
	uniform_real_distribution<> dis_arr;

	rng::tsc_seed seed_arr;
	rng::rng128 gen_arr;

	uint32_t w, w1, w2;

	uint32_t width_over_four;

	uint8_t minus_log_p = 0;
	uint8_t minus_log_p_bound = 6;

	double LogOneMinusP;

	uint32_t interval;

	uint64_t curr_sum;

	uint8_t rand_pos = 0;
	__m256i rand_bits;
	char * RandByteArray;

	uint8_t weighted_rand_pos = 0;
	__m256i weighted_rand_bits;
	uint16_t * WeightedRandByteArray;

	/* mask for parallel devision */
	uint64_t division_mask = 0xFFFEFFFEFFFEFFFE;

	inline unsigned aaeeelerated_geometric(double LogOneMinusP);

	void divide_array_counters();

	void increment_h(const char * str);

	void add_h(const char * str, int c);

public:

	CountMinAEE(int width, int height, int seed);
	~CountMinAEE();

	// Unweighted
	void increment(const char * str);

	// Weighted
	void add(const char * str, int c);

	uint64_t query(const char * str);

};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

class ConservativeUpdateAEE {

	int seed;

	int width;
	int height;

	int width_mask;

	BOBHash *bobhash;

	aee_counter_t **aee_cus;

	int* indices;

	mt19937 generator_arr;
	uniform_real_distribution<> dis_arr;

	rng::tsc_seed seed_arr;
	rng::rng128 gen_arr;

	uint32_t w, w1, w2;

	uint32_t width_over_four;

	uint8_t minus_log_p = 0;
	uint8_t minus_log_p_bound = 6;

	double LogOneMinusP;

	uint32_t interval;

	uint64_t curr_sum;

	uint8_t rand_pos = 0;
	__m256i rand_bits;
	char * RandByteArray;

	/* mask for parallel devision */
	uint64_t division_mask = 0xFFFEFFFEFFFEFFFE;

	inline unsigned aaeeelerated_geometric(double LogOneMinusP);

	void divide_array_counters();

	void increment_h(const char * str);

public:

	ConservativeUpdateAEE(int width, int height, int seed);
	~ConservativeUpdateAEE();

	void increment(const char * str);
	uint64_t query(const char * str);

};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


class CountMinBaseline {

	int width;
	int height;

	int width_mask;

	BOBHash *bobhash;

	uint32_t **baseline_cms;

public:

	CountMinBaseline();
	~CountMinBaseline();

	void initialize(int width, int height);
	void increment(const char * str);
	uint64_t query(const char * str);

};

class WeightedCountMinBaseline {

	int width;
	int height;

	int width_mask;

	BOBHash *bobhash;

	uint64_t **baseline_cms;

public:

	WeightedCountMinBaseline();
	~WeightedCountMinBaseline();

	void initialize(int width, int height);
	void add(const char * str, int c);
	uint64_t query(const char * str);

};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

class ConservativeUpdateBaseline {

	int width;
	int height;

	int width_mask;

	BOBHash *bobhash;

	uint32_t **baseline_cu;

	uint32_t** counters;

public:

	ConservativeUpdateBaseline();
	~ConservativeUpdateBaseline();

	void initialize(int width, int height);
	void increment(const char * str);
	uint64_t query(const char * str);

};
#endif // !COUNT_MIN_AEE

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

class CountMinAEE_MaxSpeed {

	uint64_t N_current, N_prime, Shifted_N_prime;

	int seed;

	int width;
	int height;

	int width_mask;

	BOBHash *bobhash;

	aee_counter_t **aee_cms;

	int* indices;

	mt19937 generator_arr;
	uniform_real_distribution<> dis_arr;

	rng::tsc_seed seed_arr;
	rng::rng128 gen_arr;

	uint32_t w, w1, w2;

	uint32_t width_over_four;

	uint8_t minus_log_p = 0;
	uint8_t minus_log_p_bound = 6;

	double LogOneMinusP;

	uint32_t interval;

	uint64_t curr_sum;

	uint8_t rand_pos = 0;
	__m256i rand_bits;
	char * RandByteArray;

	uint8_t weighted_rand_pos = 0;
	__m256i weighted_rand_bits;
	uint16_t * WeightedRandByteArray;

	/* mask for parallel devision */
	uint64_t division_mask = 0xFFFEFFFEFFFEFFFE;

	inline unsigned aaeeelerated_geometric(double LogOneMinusP);

	void divide_array_counters();

	void increment_h(const char * str);

public:

	CountMinAEE_MaxSpeed(int width, int height, int seed, double epsilon_aee, double delta_aee);
	~CountMinAEE_MaxSpeed();

	void increment(const char * str);

	uint64_t query(const char * str);

};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
