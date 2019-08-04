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
#include <unordered_map>
#include <cstring>

#include "SpaceSavingAEE.hpp"
#include "SpaceSavingHeap.hpp"
#include "SpaceSavingHeapFiveTuple.hpp"
#include "SpaceSavingHeapAEE.hpp"
#include "RapHeapFiveTuple.hpp"
#include "RAPHeapAEE.hpp"

#include "SpaceSavingTests.hpp"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void test_space_saving_heap_ft_error_on_arrival(int N, int width, int seed, const char* data)
{

	SpaceSavingHeapFT ss_heap(1.0f / width);

	unordered_map<uint64_t, uint64_t> true_sizes;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	int64_t stop_loop = N * FT_SIZE;

	long double L1e = 0, L2e = 0, L_max = 0;

	for (int64_t i = 0; i < stop_loop; i += 13)
	{
		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)ss_heap.LCL_PointEst(data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += 1;

		ss_heap.LCL_Update(data + i, 1);
	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_space_saving_heap_ft_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;
}

void test_space_saving_heap_ft_speed(int N, int width, int seed, const char* data)
{

	SpaceSavingHeapFT ss_heap(1.0f / width);

	int stop_loop = N * FT_SIZE;
	auto start = chrono::steady_clock::now();
	for (int i = 0; i < stop_loop; i += 13)
	{
		ss_heap.LCL_Update(data + i, 1);
	}
	auto end = chrono::steady_clock::now();

	auto test_aee_array_single_counter_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_space_saving_heap_ft_speed! Elapsed time in milliseconds : "
		<< test_aee_array_single_counter_time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("test_space_saving_heap_ft_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tTime\t" << test_aee_array_single_counter_time << endl;

}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void test_space_saving_heap_rap_ft_error_on_arrival(int N, int width, int seed, const char* data)
{

	RapHeapFT ss_heap_rap(1.0f / width);

	unordered_map<uint64_t, uint64_t> true_sizes;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	int64_t stop_loop = N * FT_SIZE;

	long double L1e = 0, L2e = 0, L_max = 0;

	for (int64_t i = 0; i < stop_loop; i += 13)
	{
		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)ss_heap_rap.LCL_PointEst(data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += 1;

		ss_heap_rap.LCL_Update(data + i, 1);
	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_space_saving_heap_rap_ft_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;
}

void test_space_saving_heap_rap_ft_speed(int N, int width, int seed, const char* data)
{

	RapHeapFT ss_heap_rap(1.0f / width);

	int stop_loop = N * FT_SIZE;
	auto start = chrono::steady_clock::now();
	for (int i = 0; i < stop_loop; i += 13)
	{
		ss_heap_rap.LCL_Update(data + i, 1);
	}
	auto end = chrono::steady_clock::now();

	auto test_aee_array_single_counter_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_space_saving_heap_rap_ft_speed! Elapsed time in milliseconds : "
		<< test_aee_array_single_counter_time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("test_space_saving_heap_rap_ft_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tTime\t" << test_aee_array_single_counter_time << endl;

}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void test_space_saving_heap_aee_rap_error_on_arrival(int N, int width, int seed, const char* data)
{

	RAPHeapAEE aee_rap(1.0 / width, seed);

	unordered_map<uint64_t, uint64_t> true_sizes;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	int64_t stop_loop = N * FT_SIZE;

	long double L1e = 0, L2e = 0, L_max = 0;

	for (int64_t i = 0; i < stop_loop; i += 13)
	{
		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)aee_rap.LCL_PointEst(data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += 1;

		aee_rap.Probabilistic_Increment(data + i, 1);
	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_space_saving_heap_aee_rap_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;
}
     
void test_space_saving_heap_aee_rap_speed(int N, int width, int seed, const char* data)
{

	RAPHeapAEE aee_rap(1.0 / width, seed);

	int stop_loop = N * FT_SIZE;
	auto start = chrono::steady_clock::now();
	for (int i = 0; i < stop_loop; i += 13)
	{
		aee_rap.Probabilistic_Increment(data + i, 1);
	}
	auto end = chrono::steady_clock::now();

	auto test_aee_array_single_counter_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_space_saving_heap_aee_rap_speed! Elapsed time in milliseconds : "
		<< test_aee_array_single_counter_time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("test_space_saving_heap_aee_rap_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tTime\t" << test_aee_array_single_counter_time << endl;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void test_space_saving_heap_aee_error_on_arrival(int N, int width, int seed, const char* data)
{

	SpaceSavingHeapAEE aee_spsa(1.0 / width, seed);

	unordered_map<uint64_t, uint64_t> true_sizes;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	int64_t stop_loop = N * FT_SIZE;

	long double L1e = 0, L2e = 0, L_max = 0;

	for (int64_t i = 0; i < stop_loop; i += 13)
	{
		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)aee_spsa.LCL_PointEst(data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += 1;

		aee_spsa.Probabilistic_Increment(data + i, 1);
	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_space_saving_heap_aee_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;
}


void test_space_saving_heap_aee_speed(int N, int width, int seed, const char* data)
{

	SpaceSavingHeapAEE aee_spsa(1.0 / width, seed);

	int stop_loop = N * FT_SIZE;
	auto start = chrono::steady_clock::now();
	for (int i = 0; i < stop_loop; i += 13)
	{
		aee_spsa.Probabilistic_Increment(data + i, 1);
	}
	auto end = chrono::steady_clock::now();

	auto test_aee_array_single_counter_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_space_saving_heap_aee_speed! Elapsed time in milliseconds : "
		<< test_aee_array_single_counter_time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("test_space_saving_heap_aee_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tTime\t" << test_aee_array_single_counter_time << endl;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


