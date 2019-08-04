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
#include "AEE_Counter.hpp"

using namespace std;

inline unsigned AEE_Counter::aaeeelerated_geometric(double LogOneMinusP) {
	// Return a randomly drawn geometric variable with parameter p, such that LogOneMinusP = log(1-p)
	return log(dis_arr(generator_arr)) / LogOneMinusP + 1;
}

mt19937 AEE_Counter::generator_arr;
rng::rng128 AEE_Counter::gen_arr;
uint8_t AEE_Counter::minus_log_p = 0;
uniform_real_distribution<> AEE_Counter::dis_arr(0, 1);
uint8_t AEE_Counter::rand_pos = 0;
__m256i AEE_Counter::rand_bits;
char * AEE_Counter::RandByteArray((char *)&rand_bits);
uint32_t AEE_Counter::num_instances = 0;
double AEE_Counter::LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p_bound)); 
uint32_t AEE_Counter::interval = aaeeelerated_geometric(LogOneMinusP);
bool AEE_Counter::Should_I_divide = false;


AEE_Counter::AEE_Counter() :
	val(0)
{
}

void AEE_Counter::set_seed(int seed)
{
	generator_arr.seed(seed);
	gen_arr.set_seed_rng128(seed);

	rand_bits = _mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr());
}

void AEE_Counter::increase_val()
{
	if (val == 65535)
	{
		++minus_log_p;
		divide_counter();
		Should_I_divide = true;
	
		// This is not optimal but convenient :)
		LogOneMinusP = log(1 - 1.0 / (1 << minus_log_p));
	}
	++val;
}

/*
void AEE_Counter::increment()
{
	if (minus_log_p == 0)
	{
		increase_val();
	}
	else if (minus_log_p < minus_log_p_bound)
	{
		if (RandByteArray[rand_pos >> 3] & (1 << ((rand_pos & 0x7) - 1))) // TODO: lookup table 
		{
			increase_val();
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
			rand_bits = _mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr());
			for (int j = 0; j < minus_log_p - 1; ++j)
			{
				rand_bits = _mm256_and_si256(rand_bits, _mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr()));
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
			increase_val();	
			interval = aaeeelerated_geometric(LogOneMinusP);
		}
	}
}
*/

bool AEE_Counter::sample_or_advance()
{
	bool res = false;
	if (minus_log_p == 0)
	{
		res = true;
	}
	else if (minus_log_p < minus_log_p_bound)
	{
		if (RandByteArray[rand_pos >> 3] & (1 << ((rand_pos & 0x7) - 1))) // TODO: lookup table 
		{
			res = true;
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
			rand_bits = _mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr());
			for (int j = 0; j < minus_log_p - 1; ++j)
			{
				rand_bits = _mm256_and_si256(rand_bits, _mm256_set_epi64x(gen_arr(), gen_arr(), gen_arr(), gen_arr()));
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
			res = true;
			interval = aaeeelerated_geometric(LogOneMinusP);
		}
	}
	return res;
}

uint64_t AEE_Counter::query()
{
	return (uint64_t)val << minus_log_p;
}

