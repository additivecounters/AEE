#include <iostream>
#include <stdlib.h>
#include <random>
#include <chrono>
#include <math.h>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <string>
#include <unordered_map>

#include "CountMinAEE.hpp"
#include "CountMinSketchTests.hpp"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void test_aee_cms_error_on_arrival(int N, int width, int height, int seed, const char* data)
{

	CountMinAEE aee_cms(width, height, seed);

	unordered_map<uint64_t, uint64_t> true_sizes;

	int64_t stop_loop = N * FT_SIZE;

	long double L1e = 0, L2e = 0, L_max = 0;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{

		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)aee_cms.query(data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += 1;
		aee_cms.increment(data + i);

	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_aee_cms_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;
}

void test_aee_cms_speed(int N, int width, int height, int seed, const char* data)
{

	CountMinAEE aee_cms(width, height, seed);

	int64_t stop_loop = N * FT_SIZE;

	auto start = chrono::steady_clock::now();
	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{
		aee_cms.increment(data + i);
	}
	auto end = chrono::steady_clock::now();

	auto time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_aee_cms_speed: Elapsed time in milliseconds : "
		<< time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("test_aee_cms_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tTime\t" << time << endl;
}

/* Weighted */

void test_weighted_aee_cms_error_on_arrival(int N, int width, int height, int seed, const char* data, const uint16_t* data_weights)
{

	CountMinAEE aee_wcms(width, height, seed);

	unordered_map<uint64_t, uint64_t> true_sizes;

	int64_t stop_loop = N * FT_SIZE;

	long double L1e = 0, L2e = 0, L_max = 0;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	int64_t weight_index = 0;

	int64_t sum_weights = 0;

	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{

		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)aee_wcms.query(data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += data_weights[weight_index];
		aee_wcms.add(data + i, data_weights[weight_index]);

		sum_weights += data_weights[weight_index];

		++weight_index;

	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_weighted_aee_cms_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tSum\t" << sum_weights << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;

}

void test_weighted_aee_cms_speed(int N, int width, int height, int seed, const char* data, const uint16_t* data_weights)
{

	CountMinAEE aee_wcms(width, height, seed);

	int64_t stop_loop = N * FT_SIZE;
	int64_t weight_index = -1;

	auto start = chrono::steady_clock::now();
	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{
		aee_wcms.add(data + i, data_weights[++weight_index]);
	}
	auto end = chrono::steady_clock::now();

	auto time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_weighted_aee_cms_speed: Elapsed time in milliseconds : "
		<< time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("test_weighted_aee_cms_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tTime\t" << time << endl;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


void test_aee_cus_error_on_arrival(int N, int width, int height, int seed, const char* data)
{

	ConservativeUpdateAEE aee_cus(width, height, seed);

	unordered_map<uint64_t, uint64_t> true_sizes;

	int64_t stop_loop = N * FT_SIZE;

	long double L1e = 0, L2e = 0, L_max = 0;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{

		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)aee_cus.query(data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += 1;
		aee_cus.increment(data + i);

	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_aee_cus_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;
}

void test_aee_cus_speed(int N, int width, int height, int seed, const char* data)
{

	ConservativeUpdateAEE aee_cus(width, height, seed);

	int64_t stop_loop = N * FT_SIZE;

	auto start = chrono::steady_clock::now();
	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{
		aee_cus.increment(data + i);
	}
	auto end = chrono::steady_clock::now();

	auto time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_aee_cus_speed: Elapsed time in milliseconds : "
		<< time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("test_aee_cus_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tTime\t" << time << endl;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void test_baseline_cms_error_on_arrival(int N, int width, int height, int seed, const char* data)
{

	CountMinBaseline cms_baseline;
	cms_baseline.initialize(width, height);

	unordered_map<uint64_t, uint64_t> true_sizes;

	int64_t stop_loop = N * FT_SIZE;

	long double L1e = 0, L2e = 0, L_max = 0;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	for (int64_t i = 0; i < stop_loop; i += 13)
	{

		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)cms_baseline.query(data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += 1;
		cms_baseline.increment(data + i);
	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_baseline_cms_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;
}

void test_baseline_cms_speed(int N, int width, int height, int seed, const char* data)
{

	CountMinBaseline cms_baseline;
	cms_baseline.initialize(width, height);

	int64_t stop_loop = N * FT_SIZE;

	auto start = chrono::steady_clock::now();
	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{
		cms_baseline.increment(data + i);
	}
	auto end = chrono::steady_clock::now();

	auto time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_baseline_cms_speed: Elapsed time in milliseconds : "
		<< time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("test_baseline_cms_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tTime\t" << time << endl;
}

/* Weighted */

void test_weighted_baseline_cms_error_on_arrival(int N, int width, int height, int seed, const char* data, const uint16_t* data_weights)
{

	WeightedCountMinBaseline wcms_baseline;
	wcms_baseline.initialize(width, height);

	unordered_map<uint64_t, uint64_t> true_sizes;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	int64_t stop_loop = N * FT_SIZE;

	long double L1e = 0, L2e = 0, L_max = 0;

	int64_t weight_index = 0;

	int64_t sum_weights = 0;

	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{

		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)wcms_baseline.query(data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += data_weights[weight_index];
		wcms_baseline.add(data + i, data_weights[weight_index]);

		sum_weights += data_weights[weight_index];

		++weight_index;
	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_weighted_baseline_cms_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tSum\t" << sum_weights << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;
}

void test_weighted_baseline_cms_speed(int N, int width, int height, int seed, const char* data, const uint16_t* data_weights)
{

	WeightedCountMinBaseline wcms_baseline;
	wcms_baseline.initialize(width, height);

	int64_t stop_loop = N * FT_SIZE;
	int64_t weight_index = -1;

	auto start = chrono::steady_clock::now();
	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{
		wcms_baseline.add(data + i, data_weights[++weight_index]);
	}
	auto end = chrono::steady_clock::now();

	auto time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_weighted_baseline_cms_speed: Elapsed time in milliseconds : "
		<< time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("test_weighted_baseline_cms_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tTime\t" << time << endl;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void test_baseline_cus_error_on_arrival(int N, int width, int height, int seed, const char* data)
{

	ConservativeUpdateBaseline cu_baseline;
	cu_baseline.initialize(width, height);

	unordered_map<uint64_t, uint64_t> true_sizes;

	int64_t stop_loop = N * FT_SIZE;

	long double L1e = 0, L2e = 0, L_max = 0;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	for (int64_t i = 0; i < stop_loop; i += 13)
	{

		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)cu_baseline.query(data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += 1;
		cu_baseline.increment(data + i);
	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_baseline_cu_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;
}

void test_baseline_cus_speed(int N, int width, int height, int seed, const char* data)
{

	ConservativeUpdateBaseline cu_baseline;
	cu_baseline.initialize(width, height);

	int64_t stop_loop = N * FT_SIZE;

	auto start = chrono::steady_clock::now();
	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{
		cu_baseline.increment(data + i);
	}
	auto end = chrono::steady_clock::now();

	auto time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_baseline_cu_speed: Elapsed time in milliseconds : "
		<< time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("test_baseline_cu_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tTime\t" << time << endl;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void test_max_speed_aee_cms_error_on_arrival(int N, int width, int height, int seed, double epsilon_aee, double delta_aee, const char* data)
{

	CountMinAEE_MaxSpeed ms_aee_cms(width, height, seed, epsilon_aee, delta_aee);

	unordered_map<uint64_t, uint64_t> true_sizes;

	int64_t stop_loop = N * FT_SIZE;

	long double L1e = 0, L2e = 0, L_max = 0;

	BOBHash ft_to_bobkey_1(seed);
	BOBHash ft_to_bobkey_2(seed + 17);

	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{

		uint64_t ft_key = (((uint64_t)ft_to_bobkey_1.run(data + i, FT_SIZE)) << 32) + (uint64_t)ft_to_bobkey_2.run(data + i, FT_SIZE);

		if (true_sizes.find(ft_key) == true_sizes.end())
		{
			true_sizes[ft_key] = 0;
		}

		long double point_error = (int64_t)ms_aee_cms.query(data + i) - (int64_t)true_sizes[ft_key];

		L1e += abs(point_error);
		L2e += (point_error * point_error);
		L_max = (L_max < abs(point_error)) ? abs(point_error) : L_max;

		true_sizes[ft_key] += 1;
		ms_aee_cms.increment(data + i);

	}

	L1e /= N;
	L2e /= N;
	L2e = sqrt(L2e);

	ofstream results_file;
	results_file.open("test_max_speed_aee_cms_error_on_arrival.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tL1 Error\t" << L1e << "\tL2 Error\t" << L2e << "\tL(inf) Error\t" << L_max << endl;
}

void test_max_speed_aee_cms_speed(int N, int width, int height, int seed, double epsilon_aee, double delta_aee, const char* data)
{

	CountMinAEE_MaxSpeed ms_aee_cms(width, height, seed, epsilon_aee, delta_aee);

	int64_t stop_loop = N * FT_SIZE;

	auto start = chrono::steady_clock::now();
	for (int64_t i = 0; i < stop_loop; i += FT_SIZE)
	{
		ms_aee_cms.increment(data + i);
	}
	auto end = chrono::steady_clock::now();

	auto time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "test_max_speed_aee_cms_speed: Elapsed time in milliseconds : "
		<< time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("test_max_speed_aee_cms_speed.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tWidth\t" << width << "\tHeight\t" << height << "\tTime\t" << time << endl;
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// Mega-tests


/////////////////////// SPEED /////////////////////////////////////////////////
// Run all non-weighted cms tests over different seeds
void Run_Unweighted_CMS_Tests_Over_Seeds_SPEED(int N, int low_width, int high_width, int height, int seeds, int seed, const char* data)
{
	for (int i = 0; i < seeds; ++i)
	{
		for (int j = low_width; j < high_width; ++j)
		{
			int width = (1 << j);

			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			test_aee_cms_speed(N, 2 * width, height, Seed, data);

			test_baseline_cms_speed(N, width, height, Seed, data);

			/*
			test_static_sac_cms_speed(N, 2 * width, height, Seed, data);

			test_dynamic_sac_cms_speed(N, 2 * width, height, Seed, data);
			*/
			
			cout << j << " " << i << endl;
		}
	}
}

// Run all non-weighted cus tests over different seeds
void Run_Unweighted_CUS_Tests_Over_Seeds_SPEED(int N, int low_width, int high_width, int height, int seeds, int seed, const char* data)
{
	for (int i = 0; i < seeds; ++i)
	{
		for (int j = low_width; j < high_width; ++j)
		{
			int width = (1 << j);

			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			test_aee_cus_speed(N, 2 * width, height, Seed, data);

			test_baseline_cus_speed(N, width, height, Seed, data);

			/*
			test_static_sac_cus_speed(N, 2 * width, height, Seed, data);

			test_dynamic_sac_cus_speed(N, 2 * width, height, Seed, data);
			*/
			
			cout << j << " " << i << endl;
		}
	}
}

// Run all weighted cms tests over different seeds
void Run_Weighted_CMS_Tests_Over_Seeds_SPEED(int N, int low_width, int high_width, int height, int seeds, int seed, const char* data, const uint16_t* data_weights)
{
	for (int i = 0; i < seeds; ++i)
	{
		for (int j = low_width; j < high_width; ++j)
		{
			int width = (1 << j);

			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			test_weighted_aee_cms_speed(N, 4 * width, height, Seed, data, data_weights);

			test_weighted_baseline_cms_speed(N, width, height, Seed, data, data_weights);

			//test_weighted_dynamic_sac_cms_speed(N, 4 * width, height, Seed, data, data_weights);

			cout << j << " " << i << endl;
		}
	}
}

/////////////////////// ACCURACY /////////////////////////////////////////////////
// Run all non-weighted cms tests over different seeds
void Run_Unweighted_CMS_Tests_Over_Seeds_ACCURACY(int N, int low_width, int high_width, int height, int seeds, int seed, const char* data)
{
	for (int i = 0; i < seeds; ++i)
	{
		for (int j = low_width; j < high_width; ++j)
		{
			int width = (1 << j);

			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			test_aee_cms_error_on_arrival(N, 2 * width, height, Seed, data);

			test_baseline_cms_error_on_arrival(N, width, height, Seed, data);

			/*
			test_static_sac_cms_error_on_arrival(N, 2 * width, height, Seed, data);

			test_dynamic_sac_cms_error_on_arrival(N, 2 * width, height, Seed, data);
			*/
			
			cout << j << " " << i << endl;
		}
	}
}

// Run all non-weighted cus tests over different seeds
void Run_Unweighted_CUS_Tests_Over_Seeds_ACCURACY(int N, int low_width, int high_width, int height, int seeds, int seed, const char* data)
{
	for (int i = 0; i < seeds; ++i)
	{
		for (int j = low_width; j < high_width; ++j)
		{
			int width = (1 << j);

			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			test_aee_cus_error_on_arrival(N, 2 * width, height, Seed, data);

			test_baseline_cus_error_on_arrival(N, width, height, Seed, data);
			
			/*
			test_static_sac_cus_error_on_arrival(N, 2 * width, height, Seed, data);

			test_dynamic_sac_cus_error_on_arrival(N, 2 * width, height, Seed, data);
			*/
			
			cout << j << " " << i << endl;
		}
	}
}

// Run all weighted cms tests over different seeds
void Run_Weighted_CMS_Tests_Over_Seeds_ACCURACY(int N, int low_width, int high_width, int height, int seeds, int seed, const char* data, const uint16_t* data_weights)
{
	for (int i = 0; i < seeds; ++i)
	{
		for (int j = low_width; j < high_width; ++j)
		{
			int width = (1 << j);

			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			test_weighted_aee_cms_error_on_arrival(N, 4 * width, height, Seed, data, data_weights);

			test_weighted_baseline_cms_error_on_arrival(N, width, height, Seed, data, data_weights);

			//test_weighted_dynamic_sac_cms_error_on_arrival(N, 4 * width, height, Seed, data, data_weights);

			cout << j << " " << i << endl;
		}
	}
}

// Run AEE_Max_Speed_CMS_Tests over different seeds
void Run_AEE_Max_Speed_CMS_Tests_Over_Seeds(int N, int low_width, int high_width, int height, double epsilon_aee, double delta_aee, int seeds, int seed, const char* data)
{
	for (int i = 0; i < seeds; ++i)
	{
		for (int j = low_width; j < high_width; ++j)
		{
			int width = (1 << j);

			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			test_max_speed_aee_cms_speed(N, width, height, seed, epsilon_aee, delta_aee, data);
			test_max_speed_aee_cms_error_on_arrival(N, width, height, seed, epsilon_aee, delta_aee, data);

			cout << j << " " << i << endl;
		}
	}
}


