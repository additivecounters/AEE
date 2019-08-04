#include <iostream>
#include <stdlib.h>
#include <random>
#include <chrono>
#include <math.h>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <string>

#include "RngFast.hpp"

using namespace std;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void WeightedDammyCounter_16(int N, const uint16_t* data_weights)
{

	int64_t sum = 0;
	for (int i = 0; i < N; ++i)
	{
		sum += data_weights[i];
	}

	int64_t dammy_counter = 0;
	auto start = chrono::steady_clock::now();
	for (int i = 0; i < N; ++i)
	{
		dammy_counter += data_weights[i];
	}
	auto end = chrono::steady_clock::now();

	auto baseline_time = chrono::duration_cast<chrono::microseconds>(end - start).count();

	cout << "WeightedDammyCounter_16! Elapsed time in milliseconds : "
		<< baseline_time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("WeightedDammyCounter_16.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tSum\t" << sum << "\tError\t" << sum - dammy_counter << "\tTime\t" << baseline_time << endl;
}


void DammyCounter_16(int N)
{
	int dammy_counter = 0;

	auto start = chrono::steady_clock::now();
	for (volatile int i = 0; i < N; ++i)
	{
		++dammy_counter;
	}
	auto end = chrono::steady_clock::now();

	auto baseline_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "DammyCounter_16! Elapsed time in milliseconds : "
		<< baseline_time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("DammyCounter_16.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tError\t" << N - dammy_counter << "\tTime\t" << baseline_time << endl;
}


void DammyCounter_8(int N)
{
	int dammy_counter = 0;

	auto start = chrono::steady_clock::now();
	for (volatile int i = 0; i < N; ++i)
	{
		++dammy_counter;
	}
	auto end = chrono::steady_clock::now();

	auto baseline_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "DammyCounter_8! Elapsed time in milliseconds : "
		<< baseline_time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("DammyCounter_8.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tError\t" << N - dammy_counter << "\tTime\t" << baseline_time << endl;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

inline unsigned aaeeelerated_geometric(double LogOneMinusP, double UnifZeroOne) {
	// Return a randomly drawn geometric variable with parameter p, such that LogOneMinusP = log(1-p)
	return log(UnifZeroOne) / LogOneMinusP + 1;
}


void AdditiveErrorEstimator_16(int N, int Seed)
{

	default_random_engine generator;
	uniform_real_distribution<> dis(0, 1);

	generator.seed(Seed);

	rng::tsc_seed rng_seed;
	rng::rng128 gen(rng_seed());

	uint16_t comp_counter = 0;
	uint8_t minus_log_p = 0;
	uint8_t minus_log_p_bound = 6;

	uint8_t rand_pos = 0;
	__m256i rand_bits = _mm256_set_epi64x(gen(), gen(), gen(), gen());
	char * RandByteArray = (char *)&rand_bits;

	int interval = aaeeelerated_geometric(log(1 - 1.0 / (1 << minus_log_p_bound)), dis(generator));

	auto start = chrono::steady_clock::now();
	for (int i = 0; i < N; ++i)
	{

		if (minus_log_p == 0)
		{
			if (comp_counter == 65535)
			{
				++minus_log_p;
				comp_counter = 32767;
			}
			comp_counter += 1;
		}
		else if (minus_log_p < minus_log_p_bound)
		{
			if (RandByteArray[rand_pos >> 3] & (1 << ((rand_pos & 0x7) - 1))) // TODO: lookup table 
			{
				if (comp_counter == 65535)
				{
					++minus_log_p;
					comp_counter = 32767;
				}
				comp_counter += 1;
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
				rand_bits = _mm256_set_epi64x(gen(), gen(), gen(), gen());
				for (int j = 0; j < minus_log_p - 1; ++j)
				{
					rand_bits = _mm256_and_si256(rand_bits, _mm256_set_epi64x(gen(), gen(), gen(), gen()));
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
				if (comp_counter == 65535)
				{
					++minus_log_p;
					comp_counter = 32767;
				}
				comp_counter += 1;
				interval = aaeeelerated_geometric(log(1 - 1.0 / (1 << minus_log_p)), dis(generator));
			}
		}
	}
	auto end = chrono::steady_clock::now();

	auto comp_counter_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "AdditiveErrorEstimator_16! Elapsed time in milliseconds : "
		<< comp_counter_time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("AdditiveErrorEstimator_16.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tError\t" << N - comp_counter * (1 << minus_log_p) << "\tTime\t" << comp_counter_time << endl;
}


void AdditiveErrorEstimator_8(int N, int Seed)
{

	default_random_engine generator;
	uniform_real_distribution<> dis(0, 1);

	generator.seed(Seed);

	rng::tsc_seed rng_seed;
	rng::rng128 gen(rng_seed());

	uint8_t comp_counter = 0;
	uint8_t minus_log_p = 0;
	uint8_t minus_log_p_bound = 5;

	uint8_t rand_pos = 0;
	uint64_t rand_bits = gen();
	char * RandByteArray = (char *)&rand_bits;

	int interval = aaeeelerated_geometric(log(1 - 1.0 / (1 << minus_log_p_bound)), dis(generator));

	auto start = chrono::steady_clock::now();
	for (int i = 0; i < N; ++i)
	{

		if (minus_log_p == 0)
		{
			if (comp_counter == 255)
			{
				++minus_log_p;
				comp_counter = 127;
			}
			comp_counter += 1;
		}
		else if (minus_log_p < minus_log_p_bound)
		{
			if (RandByteArray[rand_pos >> 3] & (1 << ((rand_pos & 0x7) - 1))) // TODO: lookup table 
			{
				if (comp_counter == 255)
				{
					++minus_log_p;
					comp_counter = 127;
				}
				comp_counter += 1;
			}
			else
			{
			}

			if (rand_pos < 63)
			{
				++rand_pos;
			}
			else
			{
				rand_bits = gen();
				for (int j = 0; j < minus_log_p - 1; ++j)
				{
					rand_bits = rand_bits & gen();
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
				if (comp_counter == 255)
				{
					++minus_log_p;
					comp_counter = 127;
				}
				comp_counter += 1;
				interval = aaeeelerated_geometric(log(1 - 1.0 / (1 << minus_log_p)), dis(generator));
			}
		}
	}
	auto end = chrono::steady_clock::now();

	auto comp_counter_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "AdditiveErrorEstimator_8! Elapsed time in milliseconds : "
		<< comp_counter_time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("AdditiveErrorEstimator_8.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tError\t" << N - comp_counter * (1 << minus_log_p) << "\tTime\t" << comp_counter_time << endl;
}


void WeightedAdditiveErrorEstimator_16(int N, int Seed, const uint16_t* data_weights)
{

	default_random_engine generator;
	uniform_real_distribution<> dis(0, 1);

	generator.seed(Seed);

	rng::tsc_seed rng_seed;
	rng::rng128 gen(rng_seed());

	int64_t sum = 0;
	for (int i = 0; i < N; ++i)
	{
		sum += data_weights[i];
	}

	uint8_t minus_log_p = 0;
	uint8_t minus_log_p_bound = 14;

	uint8_t sampling_mask = 0x01;

	double LogOneMinusP = log(1.0 - 1.0 / (1 << minus_log_p_bound));
	int interval = aaeeelerated_geometric(LogOneMinusP, dis(generator));

	uint16_t weighted_comp_counter = 0;

	uint32_t w, w1, w2;

	uint64_t curr_sum;

	uint8_t rand_pos = 0;
	__m256i rand_bits = _mm256_set_epi64x(gen(), gen(), gen(), gen());
	uint16_t * RandByteArray = (uint16_t *)&rand_bits;

	auto start = chrono::steady_clock::now();
	for (int i = 0; i < N; ++i)
	{

		w = data_weights[i];

		if (minus_log_p == 0)
		{

			if (weighted_comp_counter + w <= 65535)
			{
				weighted_comp_counter += w;
			}
			else
			{
				// TODO: devide p several times?
				minus_log_p = 1;
				curr_sum = weighted_comp_counter + w;
				weighted_comp_counter = curr_sum / 2;
			}
		}

		else if (minus_log_p < minus_log_p_bound)
		{

			w1 = w >> minus_log_p;
			w2 = w - (w1 << minus_log_p);

			uint32_t increment = w1;

			if (RandByteArray[rand_pos] <= w2 << (16 - minus_log_p))
			{
				increment += 1;
			}


			if (++rand_pos < 16)
			{
			}
			else
			{
				rand_bits = _mm256_set_epi64x(gen(), gen(), gen(), gen());
				RandByteArray = (uint16_t *)&rand_bits;
				rand_pos = 0;
			}


			if (weighted_comp_counter <= 65535 - increment)
			{
				weighted_comp_counter += increment;
			}
			else
			{
				// TODO: devide p several times?
				++minus_log_p;
				sampling_mask = (1 << minus_log_p) - 1;
				int curr_sum = weighted_comp_counter + increment;
				weighted_comp_counter = curr_sum / 2;
			}
		}

		else
		{

			w1 = w >> minus_log_p;
			w2 = w - (w1 << minus_log_p);

			curr_sum = weighted_comp_counter + w1;

			while (w2 >= interval)
			{
				++curr_sum;
				w2 -= interval;
				interval = aaeeelerated_geometric(LogOneMinusP, dis(generator));
			}
			interval -= w2;

			if (curr_sum <= 65535)
			{
				weighted_comp_counter = curr_sum;
			}
			else
			{
				// TODO: devide p several times?
				++minus_log_p;
				LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p));
				weighted_comp_counter = curr_sum / 2;
			}
		}

	}
	auto end = chrono::steady_clock::now();

	auto weighted_comp_counter_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "WeightedAdditiveErrorEstimator_16! Elapsed time in milliseconds : "
		<< weighted_comp_counter_time / 1000
		<< " ms" << endl;

	ofstream results_file;
	results_file.open("WeightedAdditiveErrorEstimator_16.txt", ofstream::out | ofstream::app);
	results_file << "N\t" << N << "\tSum\t" << sum << "\tError\t" << sum - (int64_t)weighted_comp_counter * (1 << minus_log_p) << "\tTime\t" << weighted_comp_counter_time << endl;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// Mega-tests

// Run all non-weighted tests over different seeds
void Run_Unweighted_Single_Counter_Tests_Over_Seeds(int low_power_2, int high_power_2, int seeds, int seed)
{
	int range = 2 * (high_power_2 - low_power_2) - 1;
	for (int j = 0; j < range; ++j)
	{
		int N = (1 << (low_power_2 + j / 2)) + (j % 2)*(1 << (low_power_2 + j / 2)) / 2;
		for (int i = 0; i < seeds; ++i)
		{
			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			AdditiveErrorEstimator_16(N, Seed);
			AdditiveErrorEstimator_8(N, Seed);

			DammyCounter_16(N);
			DammyCounter_8(N);
			
			/*
			SACounterStatic_16(N);
			SACounterStatic_8(N);

			SACounterDynamic_16(N);
			SACounterDynamic_8(N);
			*/
			 
		}
	}
}

// Run all weighted tests over different seeds
void Run_Weighted_Single_Counter_Tests_Over_Seeds(int low_power_2, int high_power_2, int seeds, int seed, const uint16_t* data_weights)
{
	int range = 2 * (high_power_2 - low_power_2) - 1;
	for (int j = 0; j < range; ++j)
	{
		int N = (1 << (low_power_2 + j / 2)) + (j % 2)*(1 << (low_power_2 + j / 2)) / 2;
		for (int i = 0; i < seeds; ++i)
		{
			int Seed = (seed + (j + 1)*(117 + i)) % 1229;

			WeightedAdditiveErrorEstimator_16(N, Seed, data_weights);
			WeightedDammyCounter_16(N, data_weights);
			//WeightedSACounterDynamic_16(N, data_weights);
		}
	}
}



