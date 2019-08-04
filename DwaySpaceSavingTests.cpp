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
#include "DwaySpaceSavingAEE.hpp"
#include "DwayRapAEE.hpp"
#include "DwayRap.hpp"

////////////////////////////////////////////////////////////////////////

void test_dway_ss_aee_error_on_arrival(int N, int seed, int d, int num_rows, const char* data)
{
	Dway_Space_Saving_AEE dw_ss_aee(seed, d, num_rows);

	unordered_map<uint64_t, uint64_t> true_sizes;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	int64_t stop_loop = N * FT_SIZE;

	long double L1e = 0, L2e = 0, L_max = 0;

	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{

		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)dw_ss_aee.query(data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += 1;

		dw_ss_aee.increment(data + i);
	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_dway_ss_aee_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\td\t" << d << "\tnum_rows\t" << num_rows << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;

}

void test_dway_ss_aee_speed(int N, int seed, int d, int num_rows, const char* data)
{
	Dway_Space_Saving_AEE dw_ss_aee(seed, d, num_rows);

	int64_t stop_loop = N * FT_SIZE;

	auto start = chrono::steady_clock::now();
	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{
		dw_ss_aee.increment(data + i);
	}
	auto end = chrono::steady_clock::now();

	auto test_aee_array_single_counter_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_dway_ss_aee_speed! Elapsed time in milliseconds : "
		<< test_aee_array_single_counter_time / 1000
		<< " ms" << endl;
	cout << endl;

	ofstream results_file;
	results_file.open("test_dway_ss_aee_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\td\t" << d << "\tnum_rows\t" << num_rows << "\tTime\t" << test_aee_array_single_counter_time << endl;

}

////////////////////////////////////////////////////////////////////////

void test_dway_rap_aee_error_on_arrival(int N, int seed, int d, int num_rows, const char* data)
{
	Dway_Rap_AEE dw_ss_aee(seed, d, num_rows);

	unordered_map<uint64_t, uint64_t> true_sizes;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	int64_t stop_loop = N * FT_SIZE;

	long double L1e = 0, L2e = 0, L_max = 0;

	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{

		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)dw_ss_aee.query(data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += 1;

		dw_ss_aee.increment(data + i);
	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_dway_rap_aee_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\td\t" << d << "\tnum_rows\t" << num_rows << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;

}

void test_dway_rap_aee_speed(int N, int seed, int d, int num_rows, const char* data)
{
	Dway_Rap_AEE dw_ss_aee(seed, d, num_rows);

	int64_t stop_loop = N * FT_SIZE;

	auto start = chrono::steady_clock::now();
	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{
		dw_ss_aee.increment(data + i);
	}
	auto end = chrono::steady_clock::now();

	auto test_aee_array_single_counter_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_dway_rap_aee_speed! Elapsed time in milliseconds : "
		<< test_aee_array_single_counter_time / 1000
		<< " ms" << endl;
	cout << endl;

	ofstream results_file;
	results_file.open("test_dway_rap_aee_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\td\t" << d << "\tnum_rows\t" << num_rows << "\tTime\t" << test_aee_array_single_counter_time << endl;

}

////////////////////////////////////////////////////////////////////////

void test_dway_rap_error_on_arrival(int N, int seed, int d, int num_rows, const char* data)
{
	Dway_Rap dwr(seed, d, num_rows);

	unordered_map<uint64_t, uint64_t> true_sizes;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	int64_t stop_loop = N * FT_SIZE;

	identifier_t cast_data = (identifier_t)data;

	long double L1e = 0, L2e = 0, L_max = 0;

	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{

		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)dwr.query(cast_data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += 1;

		dwr.increment(cast_data + i);
	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_dway_rap_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\td\t" << d << "\tnum_rows\t" << num_rows << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;

}

void test_dway_rap_speed(int N, int seed, int d, int num_rows, const char* data)
{
	Dway_Rap dwr(seed, d, num_rows);

	int64_t stop_loop = N * FT_SIZE;

	identifier_t cast_data = (identifier_t)data;

	auto start = chrono::steady_clock::now();
	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{
		dwr.increment(cast_data + i);
	}
	auto end = chrono::steady_clock::now();

	auto test_aee_array_single_counter_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_dway_rap_speed! Elapsed time in milliseconds : "
		<< test_aee_array_single_counter_time / 1000
		<< " ms" << endl;
	cout << endl;

	ofstream results_file;
	results_file.open("test_dway_rap_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\td\t" << d << "\tnum_rows\t" << num_rows << "\tTime\t" << test_aee_array_single_counter_time << endl;
}
