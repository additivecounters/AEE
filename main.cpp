#include <iostream>
#include <stdlib.h>
#include <random>
#include <chrono>
#include <math.h>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <sstream>
#include <string>
#include <cstring>
#include <unordered_map>

#include "Defs.hpp"

#include "SingleCounterTests.hpp"
#include "CountMinSketchTests.hpp"
#include "DwaySpaceSavingTests.hpp"
#include "SpaceSavingTests.hpp"

using namespace std;

// Cache SPEED
void Run_Space_Saving_Speed(int N, int low_count, int high_count, int seeds, int seed, const char* data)
{
	for (int i = 0; i < seeds; ++i)
	{
		for (int j = low_count; j < high_count; ++j)
		{
			int width = (1 << j);

			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			test_space_saving_heap_ft_speed(N, width, Seed, data);
			test_space_saving_heap_aee_speed(N, width, Seed, data);

			cout << j << " " << i << endl;
		}
	}
}

void Run_RAP_Speed(int N, int low_count, int high_count, int seeds, int seed, const char* data)
{
	for (int i = 0; i < seeds; ++i)
	{
		for (int j = low_count; j < high_count; ++j)
		{
			int width = (1 << j);

			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			test_space_saving_heap_rap_ft_speed(N, width, Seed, data);
			test_space_saving_heap_aee_rap_speed(N, width, Seed, data);

			cout << j << " " << i << endl;
		}
	}
}

void Run_Dway_RAP_Speed(int N, int low_count, int high_count, int d, int seeds, int seed, const char* data)
{
	for (int i = 0; i < seeds; ++i)
	{
		for (int j = low_count; j < high_count; ++j)
		{
			int width = (1 << j);

			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			test_dway_rap_speed(N, Seed, d, width, data);
			test_dway_rap_aee_speed(N, Seed, d, width, data);

			cout << j << " " << i << endl;
		}
	}
}

// Cache ACCURACY
void Run_Space_Saving_ACCURACY(int N, int low_count, int high_count, int seeds, int seed, const char* data)
{
	for (int i = 0; i < seeds; ++i)
	{
		for (int j = low_count; j < high_count; ++j)
		{
			int width = (1 << j);

			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			test_space_saving_heap_ft_error_on_arrival(N, width, Seed, data);
			test_space_saving_heap_aee_error_on_arrival(N, width, Seed, data);

			cout << j << " " << i << endl;
		}
	}
}

void Run_RAP_ACCURACY(int N, int low_count, int high_count, int seeds, int seed, const char* data)
{
	for (int i = 0; i < seeds; ++i)
	{
		for (int j = low_count; j < high_count; ++j)
		{
			int width = (1 << j);

			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			test_space_saving_heap_rap_ft_error_on_arrival(N, width, Seed, data);
			test_space_saving_heap_aee_rap_error_on_arrival(N, width, Seed, data);

			cout << j << " " << i << endl;
		}
	}
}

void Run_Dway_RAP_ACCURACY(int N, int low_count, int high_count, int d, int seeds, int seed, const char* data)
{
	for (int i = 0; i < seeds; ++i)
	{
		for (int j = low_count; j < high_count; ++j)
		{
			int width = (1 << j);

			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			test_dway_rap_error_on_arrival(N, Seed, d, width, data);
			test_dway_rap_aee_error_on_arrival(N, Seed, d, width, data);

			cout << j << " " << i << endl;
		}
	}
}

int main(int argc, char* argv[])
{

	if (argc < 6) {
		cout << "Usage Error:\n";
		cout << "argv[1]: int N\n";
		cout << "argv[2]: int Seed\n";
		cout << "argv[3]: int Alg\n";
		cout << "argv[4]: bool weighted_experiment\n";
		cout << "argv[5]: String Trace\n";
		system("pause");
		return 0;
	}

	// Read arguments
	int N = atoi(argv[1]);
	int Seed = atoi(argv[2]);
	int Alg = atoi(argv[3]);
	bool weighted_experiment = (atoi(argv[4]) == 0) ? false : true;
	string Trace = string(argv[5]);

	// Print input arguments
	cout << "Received input arguments:" << endl;
	cout << "argv[1]: int N = " << N << endl;
	cout << "argv[2]: int Seed = " << Seed << endl;
	cout << "argv[3]: int Alg = " << Alg << endl;
	cout << "argv[4]: bool weighted_experiment = " << weighted_experiment << endl;
	cout << "argv[5]: String Trace = \"" << Trace << "\"" << endl;
	cout << endl;

	if (weighted_experiment)
	{
		Trace.append("_weighted");
	}

	// Asserts
	assert((Seed < 1229) && "Seed too large for BobHash");

	// Seed 
	srand(Seed);

	// Read trace?
	ifstream f(Trace, ios::binary);

	char* data = NULL;
	uint16_t* data_weights = NULL;

	int64_t read_so_far = 0;
	int64_t remaining = N;

	data = new char[FT_SIZE * N]();

	if (weighted_experiment)
	{
		assert((N <= 98000000) && "N too large for used traces");

		data_weights = new uint16_t[N]();
		char* temp_data_buffer = new char[100000 * FT_SIZE_WEIGHTS]();

		while (remaining > 0)
		{
			int64_t to_read = remaining > 100000 ? 100000 : remaining;
			f.read(temp_data_buffer, to_read * FT_SIZE_WEIGHTS);

			for (int i = 0; i < to_read; ++i)
			{
				memcpy(data + (i + read_so_far)* FT_SIZE, temp_data_buffer + i * FT_SIZE_WEIGHTS, FT_SIZE);

				uint8_t* p1 = (uint8_t*)(temp_data_buffer + FT_SIZE_WEIGHTS * i + FT_SIZE);
				uint8_t* p2 = (uint8_t*)(temp_data_buffer + FT_SIZE_WEIGHTS * i + FT_SIZE + 1);
				data_weights[read_so_far + i] = (*p1) * 256 + *p2;
			}

			remaining -= to_read;
			read_so_far += to_read;
		}

		delete[] temp_data_buffer;
	}

	else
	{
		while (remaining > 0)
		{
			int64_t to_read = remaining > 100000 ? 100000 : remaining;
			f.read(data + read_so_far * FT_SIZE, to_read * FT_SIZE);
			remaining -= to_read;
			read_so_far += to_read;
		}
	}

	assert((read_so_far == N) && "Read trace file error");
	assert((remaining == 0) && "Read trace file error");

	// choose algorithm
	switch (Alg) {
		case 0:
		{
			if (weighted_experiment)
			{
				Run_Weighted_Single_Counter_Tests_Over_Seeds(5, 26, 10, Seed, data_weights);
			}
			else
			{				
				Run_Unweighted_Single_Counter_Tests_Over_Seeds(5, 30, 10, Seed);
			}
			break;
		}
		case 1:
		{
			if (weighted_experiment)
			{
				Run_Weighted_CMS_Tests_Over_Seeds_SPEED(N, 7, 17, 3, 10, Seed, data, data_weights);
				Run_Weighted_CMS_Tests_Over_Seeds_ACCURACY(N, 7, 17, 3, 10, Seed, data, data_weights);
			}
			else
			{
				Run_Unweighted_CMS_Tests_Over_Seeds_SPEED(N, 8, 18, 3, 10, Seed, data);
				Run_Unweighted_CMS_Tests_Over_Seeds_ACCURACY(N, 8, 18, 3, 10, Seed, data);
			}
			break;
		}
		case 2:
		{

			Run_Unweighted_CUS_Tests_Over_Seeds_SPEED(N, 8, 18, 3, 10, Seed, data);
			Run_Unweighted_CUS_Tests_Over_Seeds_ACCURACY(N, 8, 18, 3, 10, Seed, data);
			break;
		}
		case 3:
		{
			Run_Space_Saving_Speed(N, 6, 21, 10, Seed, data);
			break;
		}
		case 4:
		{
			Run_RAP_Speed(N, 6, 21, 10, Seed, data);
			break;
		}
		case 5:
		{
			Run_Dway_RAP_Speed(N, 2, 17, 16, 10, Seed, data);
			break;
		}
		case 6:
		{
			Run_Space_Saving_ACCURACY(N, 6, 21, 10, Seed, data);
			break;
		}
		case 7:
		{
			Run_RAP_ACCURACY(N, 6, 21, 10, Seed, data);
			break;
		}
		case 8:
		{
			Run_Dway_RAP_ACCURACY(N, 2, 17, 16, 10, Seed, data);
			break;
		}
		case 9:
		{
			Run_AEE_Max_Speed_CMS_Tests_Over_Seeds(N, 8, 18, 3, 0.02, 0.001, 10, Seed, data);
			break;
		}
	}

	delete[] data;
	delete[] data_weights;

	return 0;

}

