// Unit tests

#include "unity.h"
#include <assert.h>
#include "protection_overload.h"

#define TEST_MAX_TIME   3600.0  // [s] max time for test execution (default = 1 hour)

// ! Test with lower tolerance
#define protectionTolerance 0.1f        // 10% tolerance for protection trip time (used in tests)
//#define protectionTolerance 0.01f     // 1% tolerance for protection trip time (used in tests)


// Test case structure
typedef struct {
    unsigned int id;
    float current;
    ProtectionOverloadState expected_state;
    float expected_time;
    const char* description;
} t_test_case;

// Test current value (mocked sensor value)
float test_current = 0.0f;

/* ------------------------------------------------ 
        Unity setup and teardown functions
   ------------------------------------------------ */

void setUp(void) { 
    // Called before every test
}

void tearDown(void) { 
    // Called after every test
}

/* ------------------------------------------------ 
        Mocked Sensor Read Function
   ------------------------------------------------ */

float Sensor_Read() {

    return test_current;
}

/* ------------------------------------------------ 
        Test Functions
   ------------------------------------------------ */

void test_ProtectionOverload_Generic(
    ProtectionOverloadParams *params,
    float simulated_current,
    ProtectionOverloadState expected_state,
    float expected_time) {

    // Init state machine and parameters
    ProtectionOverload_SM_Init(params);

    // Max test time
    float max_test_time = TEST_MAX_TIME;
    if (max_test_time < 2*expected_time) {
        max_test_time = 2*expected_time;
    }

    // Set test current value
    test_current = simulated_current;

    // Init test iterations
    int iterations = 0;
    int max_iterations = max_test_time / ProtectionOverload_SM_GetCallRate();

    // Actual testing loop
    // ! Exit conditions are only a tripped protection or maximum test time reached
    while (ProtectionOverload_SM_GetState() != ST_OVERLOAD_TRIGGERED && iterations < max_iterations) {

        // Run State Machine
        ProtectionOverload_SM_Run();

        // Increase test iterations
        iterations++;
    }

    // Check expected state using Unity
    TEST_ASSERT_EQUAL_MESSAGE(expected_state, ProtectionOverload_SM_GetState(), "Protection state mismatch.");

    // Get actual time
    float actual_time = iterations * ProtectionOverload_SM_GetCallRate();

    // Check expected timing using Unity, only in case of tripped protection
    if (expected_state == ST_OVERLOAD_TRIGGERED) {

        // print tripping energy
        // printf("Tripping time: %f\n", actual_time);
        
        TEST_ASSERT_FLOAT_WITHIN_MESSAGE(expected_time * protectionTolerance, expected_time, actual_time, "Protection time mismatch.");
    }

}

/* ------------------------------------------------ 
        Test Parameters
   ------------------------------------------------ */

// Protection parameters
// ! These parameters are shared among all test cases
ProtectionOverloadParams protectionParams = {
    .overload_threshold = 1.0f,     // ! Normalized to 1.0 (current passed in the test is actually I/Ithreshold)
    .k_factor = 1.0f,               // IEC 60947-2 protection k
    .cooling_rate = 0.98f,          
    .max_energy = 1.0f              // 1.0 is the trip threshold
};

/* ------------------------------------------------ 
        Test Case Launch Function
   ------------------------------------------------ */

void test_case_launch(const t_test_case *test_case) {
    // printf("Running test case %d: %s\n", test_case->id, test_case->description);
    
    // lunche the actual test
    test_ProtectionOverload_Generic(
        &protectionParams,
        test_case->current, 
        test_case->expected_state, 
        test_case->expected_time);
}   

/* ------------------------------------------------ 
        Test Cases - Fixed Current Values
   ------------------------------------------------ */

const t_test_case test_cases_fixed_current[] = {
    {.id = 100, .current = 0.2f,  .expected_state = ST_IDLE, .description = "Low current"},
    {.id = 101, .current = 0.8f,  .expected_state = ST_IDLE, .description = "Normal current"},
    {.id = 102, .current = 1.0f,  .expected_state = ST_IDLE, .description = "Nominal current"},
    {.id = 103, .current = 1.5f,  .expected_state = ST_OVERLOAD_TRIGGERED, .expected_time = 0.8f, .description = "1.5 times Nominal current"},
    {.id = 104, .current = 10.0f, .expected_state = ST_OVERLOAD_TRIGGERED, .expected_time = 0.02f, .description = "10 times Nominal current"},
 };

void test_fixed_current_100(void) {test_case_launch(&test_cases_fixed_current[0]);}
void test_fixed_current_101(void) {test_case_launch(&test_cases_fixed_current[1]);}
void test_fixed_current_102(void) {test_case_launch(&test_cases_fixed_current[2]);}
void test_fixed_current_103(void) {test_case_launch(&test_cases_fixed_current[3]);}
void test_fixed_current_104(void) {test_case_launch(&test_cases_fixed_current[4]);}

/* ------------------------------------------------ 
        Main Function
   ------------------------------------------------ */  

int main() {

    UNITY_BEGIN();

    // Test cases with fixed current values
    printf("\nProtection Overload Test with fixed currents\n");
    RUN_TEST(test_fixed_current_100);
    RUN_TEST(test_fixed_current_101);
    RUN_TEST(test_fixed_current_102);
    RUN_TEST(test_fixed_current_103);
    RUN_TEST(test_fixed_current_104);

    return UNITY_END();    
}
