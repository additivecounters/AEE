#pragma once

#include <iostream>
#include <stdlib.h>
#include <random>
#include <chrono>
#include <math.h>
#include <fstream>
#include <assert.h>
#include <time.h>

#include "RngFast.hpp"

using namespace std;

class AEE_Counter {

	static uint32_t num_instances;

	static mt19937 generator_arr;
	static uniform_real_distribution<> dis_arr;

	static rng::tsc_seed seed_arr;
	static rng::rng128 gen_arr;

	static uint8_t minus_log_p;
	static const uint8_t minus_log_p_bound = 6;

	static double LogOneMinusP;

	static uint32_t interval;

	static uint8_t rand_pos;
	static __m256i rand_bits;
	static char * RandByteArray;

	uint16_t val;

	static inline unsigned aaeeelerated_geometric(double LogOneMinusP);

public:

	inline void divide_counter() { val >>= 1; }

	static bool Should_I_divide;
	static void set_seed(int seed);

	AEE_Counter();

	//void increment();
	uint64_t query();

	inline uint16_t return_actual_val() { return val; }

	static bool sample_or_advance();

	void increase_val();

};