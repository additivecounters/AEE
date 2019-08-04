#ifndef SINGLE_COUNTER_TESTS
#define SINGLE_COUNTER_TESTS

// Baseline
void WeightedDammyCounter_16(int N, const uint16_t* data_weights);
void DammyCounter_16(int N);
void DammyCounter_8(int N);

/*
// Static SAC
void SACounterStatic_16(int N);
void SACounterStatic_8(int N);

// Dynamic SAC
void SACounterDynamic_16(int N);
void SACounterDynamic_8(int N);
void WeightedSACounterDynamic_16(int N, const uint16_t* data_weights);
*/

// Our
void AdditiveErrorEstimator_16(int N, int Seed);
void AdditiveErrorEstimator_8(int N, int Seed);
void WeightedAdditiveErrorEstimator_16(int N, int Seed, const uint16_t* data_weights);

// Run all non-weighted tests over different seeds
void Run_Unweighted_Single_Counter_Tests_Over_Seeds(int low_power_2, int high_power_2, int seeds, int seed);

// Run all weighted tests over different seeds
void Run_Weighted_Single_Counter_Tests_Over_Seeds(int low_power_2, int high_power_2, int seeds, int seed, const uint16_t* data_weights);

#endif // !SINGLE_COUNTER_TESTS
