#include <iostream>
#include <stdlib.h>
#include <random>
#include <chrono>
#include <math.h>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "DwayRap.hpp"

using namespace std;

Dway_Rap::Dway_Rap(int seed, int d, int num_rows) : bob_way(seed), gen_arr(seed)
{

	cnt_arrays = new counter_t*[num_rows];
	id_arrays = new identifier_t*[num_rows];

	for (int i = 0; i < num_rows; ++i)
	{
		cnt_arrays[i] = new counter_t[d]();
		id_arrays[i] = new identifier_t[d]();
		for (int j=0;j<d;++j) // need to change if identifier_t != char* (e.g., for counting source IPs)
		{
			id_arrays[i][j] = new char[FT_SIZE]; // need to chage if not 5-tuple
		}
	}

	this->d = d;
	this->num_rows = num_rows;
}

void Dway_Rap::increment(const identifier_t id)
{

	uint32_t way = bob_way.run(id, FT_SIZE) % num_rows;

	counter_t* cnt_array = cnt_arrays[way];
	identifier_t* id_array = id_arrays[way];

	int index = find_id(id, id_array);
	
	if (index == -1)
	{
		// RAP
		index = find_min(cnt_array);
		double p = 1.0/(cnt_array[index]);
				
		if (gen_arr() > UINT64_MAX*p)
		{
			return;
		}
		else
		{
			memcpy(id_array[index], id, FT_SIZE);
		}
	}
	
	++cnt_array[index];

}

counter_t Dway_Rap::query(const identifier_t id)
{

	uint32_t way = bob_way.run(id, FT_SIZE) % num_rows; 

	counter_t* cnt_array = cnt_arrays[way];
	identifier_t* id_array = id_arrays[way];

	int index = find_id(id, id_array);

	if (index == -1)
	{
		return 0;
	}

	return cnt_arrays[way][index];
}


int Dway_Rap::find_id(identifier_t id, identifier_t* id_array)
{	
	for (int i = 0; i < d; ++i)
	{
		if (memcmp(id, id_array[i], FT_SIZE) == 0) // -> if (id_array[i] == id)
		{
			return i;
		}
	}
	return -1;
}


int Dway_Rap::find_min(counter_t* cnt_array)
{
	counter_t min = cnt_array[0];
	int min_index = 0;
	
	for (int i = 1; i < d; ++i)
	{
		if (cnt_array[i] < min)
		{
			min = cnt_array[i];
			min_index = i;
		}
	}
	return min_index;
}
