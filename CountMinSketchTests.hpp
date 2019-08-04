#ifndef CMS_TESTS
#define CMS_TESTS

/* Unweighted */

// cms max-speed aee
void test_max_speed_aee_cms_error_on_arrival(int N, int width, int height, int seed, double epsilon_aee, double delta_aee, const char* data);
void test_max_speed_aee_cms_speed(int N, int width, int height, int seed, double epsilon_aee, double delta_aee, const char* data);

// cms
void test_baseline_cms_error_on_arrival(int N, int width, int height, int seed, const char* data);
void test_baseline_cms_speed(int N, int width, int height, int seed, const char* data);

void test_aee_cms_error_on_arrival(int N, int width, int height, int seed, const char* data);
void test_aee_cms_speed(int N, int width, int height, int seed, const char* data);

/*
void test_static_sac_cms_error_on_arrival(int N, int width, int height, int seed, const char* data);
void test_static_sac_cms_speed(int N, int width, int height, int seed, const char* data);

void test_dynamic_sac_cms_error_on_arrival(int N, int width, int height, int seed, const char* data);
void test_dynamic_sac_cms_speed(int N, int width, int height, int seed, const char* data);
*/

// cus
void test_baseline_cus_error_on_arrival(int N, int width, int height, int seed, const char* data);
void test_baseline_cus_speed(int N, int width, int height, int seed, const char* data);

void test_aee_cus_error_on_arrival(int N, int width, int height, int seed, const char* data);
void test_aee_cus_speed(int N, int width, int height, int seed, const char* data);

/*
void test_static_sac_cus_error_on_arrival(int N, int width, int height, int seed, const char* data);
void test_static_sac_cus_speed(int N, int width, int height, int seed, const char* data);

void test_dynamic_sac_cus_error_on_arrival(int N, int width, int height, int seed, const char* data);
void test_dynamic_sac_cus_speed(int N, int width, int height, int seed, const char* data);
*/

/* Weighted */

void test_weighted_baseline_cms_error_on_arrival(int N, int width, int height, int seed, const char* data, const uint16_t* data_weights);
void test_weighted_baseline_cms_speed(int N, int width, int height, int seed, const char* data, const uint16_t* data_weights);

void test_weighted_aee_cms_error_on_arrival(int N, int width, int height, int seed, const char* data, const uint16_t* data_weights);
void test_weighted_aee_cms_speed(int N, int width, int height, int seed, const char* data, const uint16_t* data_weights);

/*
void test_weighted_dynamic_sac_cms_error_on_arrival(int N, int width, int height, int seed, const char* data, const uint16_t* data_weights);
void test_weighted_dynamic_sac_cms_speed(int N, int width, int height, int seed, const char* data, const uint16_t* data_weights);
*/

// Run all non-weighted cms tests over different seeds
void Run_Unweighted_CMS_Tests_Over_Seeds_SPEED(int N, int low_width, int high_width, int height, int seeds, int seed, const char* data);
void Run_Unweighted_CMS_Tests_Over_Seeds_ACCURACY(int N, int low_width, int high_width, int height, int seeds, int seed, const char* data);

// Run all non-weighted cus tests over different seeds
void Run_Unweighted_CUS_Tests_Over_Seeds_SPEED(int N, int low_width, int high_width, int height, int seeds, int seed, const char* data);
void Run_Unweighted_CUS_Tests_Over_Seeds_ACCURACY(int N, int low_width, int high_width, int height, int seeds, int seed, const char* data);

// Run all weighted cms tests over different seeds
void Run_Weighted_CMS_Tests_Over_Seeds_SPEED(int N, int low_width, int high_width, int height, int seeds, int seed, const char* data, const uint16_t* data_weights);
void Run_Weighted_CMS_Tests_Over_Seeds_ACCURACY(int N, int low_width, int high_width, int height, int seeds, int seed, const char* data, const uint16_t* data_weights);

// Run AEE_Max_Speed_CMS_Tests over different seeds
void Run_AEE_Max_Speed_CMS_Tests_Over_Seeds(int N, int low_width, int high_width, int height, double epsilon_aee, double delta_aee, int seeds, int seed, const char* data);

#endif // !CMS_TESTS
