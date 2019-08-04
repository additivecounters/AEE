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

class Dway_Rap {

	BOBHash bob_way;

	rng::rng128 gen_arr;

	counter_t** cnt_arrays = NULL;

	identifier_t** id_arrays = NULL;

	int find_id(identifier_t id, identifier_t* way);
	int find_min(counter_t* way);
	
	int d, num_rows;

public:

	Dway_Rap(int seed, int d, int num_rows);

	void increment(const identifier_t id);
	counter_t query(const identifier_t id);

};
