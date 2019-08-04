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
#include "BobHash.hpp"
#include "Defs.hpp"

using namespace std;

class Dway_Space_Saving_AEE {

	BOBHash bob_way, bob_fp;

	mt19937 generator_arr;
	uniform_real_distribution<> dis_arr;

	rng::tsc_seed seed_arr;
	rng::rng128 gen_arr;

	uint32_t w, w1, w2;

	uint32_t d, d_over_four;

	uint32_t num_rows;

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

	/* array */
	uint16_t** aee_arrays = NULL;

	uint32_t** aee_fingerprints = NULL;

	inline unsigned aaeeelerated_geometric(double LogOneMinusP);
	void divide_array_counters();

	int find_fingerprint(uint32_t fp, uint32_t* way);
	int find_min(uint16_t* way);
	void increment_h(const char * str);

public:

	Dway_Space_Saving_AEE(int seed, int d, int num_rows);

	void increment(const char * str);
	uint64_t query(const char * str);

};
