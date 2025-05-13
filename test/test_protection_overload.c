// Unit tests

#include "unity.h"
#include <assert.h>
#include "protection_overload.h"

#define TEST_MAX_TIME   3600.0  // [s] max time for test execution (default = 1 hour)
#define END_SIMULATION  -1.0f   // End of simulated current values

// Simulated current single element
typedef struct {
    float time;
    float current;
} t_simulated_current_element;

// ! Test with lower tolerance
#define protectionTolerance 0.1f        // 10% tolerance for protection trip time (used in tests)
//#define protectionTolerance 0.01f     // 1% tolerance for protection trip time (used in tests)

// Test case structure
typedef struct {
    unsigned int id;
    float current;
    const t_simulated_current_element *variable_currents;
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

float Sensor_Read_Variable_Current(const t_simulated_current_element *variable_current, float time) {
    
    float current = 0.0f;
    
    // Find the current value for the given time
    for (int i = 0; variable_current[i].time != END_SIMULATION; i++) {
        if (time >= variable_current[i].time) {
            current = variable_current[i].current;
        }
    }
    return current;
}

/* ------------------------------------------------ 
        Test Functions
   ------------------------------------------------ */

void test_ProtectionOverload_Generic(
    ProtectionOverloadParams *params,
    float simulated_current,
    const t_simulated_current_element *variable_currents,
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

        // Set test current value
        if (variable_currents != NULL) {

            // Get current time
            float current_time = iterations * ProtectionOverload_SM_GetCallRate();
        
            // Get current value from simulated values
            test_current = Sensor_Read_Variable_Current(variable_currents, current_time);
        }

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
        test_case->variable_currents,
        test_case->expected_state, 
        test_case->expected_time);
}   

/* ------------------------------------------------ 
        Test Cases - Fixed Current Values
   ------------------------------------------------ */

const t_test_case test_cases_fixed_current[] = {
    {.id = 100, .current = 0.2f, .expected_state = ST_IDLE, .description = "Low current"},
    {.id = 101, .current = 0.8f, .expected_state = ST_IDLE, .description = "Normal current"},
    {.id = 102, .current = 1.0f, .expected_state = ST_IDLE, .description = "Nominal current"},
    {.id = 103, .current = 1.2f, .expected_state = ST_OVERLOAD_TRIGGERED, .expected_time = 2.27f, .description = "Overload current 1,2 x Itrip"},
    {.id = 104, .current = 1.4f, .expected_state = ST_OVERLOAD_TRIGGERED, .expected_time = 1.04f, .description = "Overload current 1,4 x Itrip"},
    {.id = 105, .current = 1.6f, .expected_state = ST_OVERLOAD_TRIGGERED, .expected_time = 0.64f, .description = "Overload current 1,6 x Itrip"},
    {.id = 106, .current = 2.0f, .expected_state = ST_OVERLOAD_TRIGGERED, .expected_time = 0.33f, .description = "Overload current 2,0 x Itrip"},
    {.id = 107, .current = 3.0f, .expected_state = ST_OVERLOAD_TRIGGERED, .expected_time = 0.12f, .description = "Overload current 3,0 x Itrip"}
 };

void test_fixed_current_100(void) {test_case_launch(&test_cases_fixed_current[0]);}
void test_fixed_current_101(void) {test_case_launch(&test_cases_fixed_current[1]);}
void test_fixed_current_102(void) {test_case_launch(&test_cases_fixed_current[2]);}
void test_fixed_current_103(void) {test_case_launch(&test_cases_fixed_current[3]);}
void test_fixed_current_104(void) {test_case_launch(&test_cases_fixed_current[4]);}
void test_fixed_current_105(void) {test_case_launch(&test_cases_fixed_current[5]);}
void test_fixed_current_106(void) {test_case_launch(&test_cases_fixed_current[6]);}
void test_fixed_current_107(void) {test_case_launch(&test_cases_fixed_current[7]);}

/* ------------------------------------------------ 
        Test Cases - Variable Current Values
   ------------------------------------------------ */

const t_simulated_current_element simulated_currents_200[] = {
    {.time = 1.0f, .current = 0.2f},
    {.time = END_SIMULATION, .current = 0.0f}
};

const t_simulated_current_element simulated_currents_201[] = {
    {.time = 1.0f, .current = 0.8f},
    {.time = END_SIMULATION, .current = 0.0f}
};

const t_simulated_current_element simulated_currents_202[] = {
    {.time = 1.0f, .current = 1.0f},
    {.time = END_SIMULATION, .current = 0.0f}
};

const t_simulated_current_element simulated_currents_203[] = {
    {.time = 1.0f, .current = 1.2f},
    {.time = END_SIMULATION, .current = 0.0f}
};

const t_simulated_current_element simulated_currents_204[] = {
    {.time = 1.0f, .current = 1.4f},
    {.time = END_SIMULATION, .current = 0.0f}
};

const t_simulated_current_element simulated_currents_205[] = {
    {.time = 1.0f, .current = 1.6f},
    {.time = END_SIMULATION, .current = 0.0f}
};

const t_simulated_current_element simulated_currents_206[] = {
    {.time = 1.0f, .current = 2.0f},
    {.time = END_SIMULATION, .current = 0.0f}
};

const t_simulated_current_element simulated_currents_207[] = {
    {.time = 1.0f, .current = 3.0f},
    {.time = END_SIMULATION, .current = 0.0f}
};

const t_test_case test_cases_variable_current[] = {
    {.id = 200, .current = 0.2f, .variable_currents = simulated_currents_200, .expected_state = ST_IDLE, .description = "Low current"},
    {.id = 201, .current = 0.8f, .variable_currents = simulated_currents_201, .expected_state = ST_IDLE, .description = "Normal current"},
    {.id = 202, .current = 1.0f, .variable_currents = simulated_currents_202, .expected_state = ST_IDLE, .description = "Nominal current"},
    {.id = 203, .current = 1.2f, .variable_currents = simulated_currents_203, .expected_state = ST_OVERLOAD_TRIGGERED, .expected_time = 3.27f, .description = "Overload current 1,2 x Itrip"},
    {.id = 204, .current = 1.4f, .variable_currents = simulated_currents_204, .expected_state = ST_OVERLOAD_TRIGGERED, .expected_time = 2.04f, .description = "Overload current 1,4 x Itrip"},
    {.id = 205, .current = 1.6f, .variable_currents = simulated_currents_205, .expected_state = ST_OVERLOAD_TRIGGERED, .expected_time = 1.64f, .description = "Overload current 1,6 x Itrip"},
    {.id = 206, .current = 2.0f, .variable_currents = simulated_currents_206, .expected_state = ST_OVERLOAD_TRIGGERED, .expected_time = 1.33f, .description = "Overload current 2,0 x Itrip"},
    {.id = 207, .current = 3.0f, .variable_currents = simulated_currents_207, .expected_state = ST_OVERLOAD_TRIGGERED, .expected_time = 1.12f, .description = "Overload current 3,0 x Itrip"}, 
};

void test_variable_current_200(void) {test_case_launch(&test_cases_variable_current[0]);}
void test_variable_current_201(void) {test_case_launch(&test_cases_variable_current[1]);}
void test_variable_current_202(void) {test_case_launch(&test_cases_variable_current[2]);}
void test_variable_current_203(void) {test_case_launch(&test_cases_variable_current[3]);}
void test_variable_current_204(void) {test_case_launch(&test_cases_variable_current[4]);}
void test_variable_current_205(void) {test_case_launch(&test_cases_variable_current[5]);}
void test_variable_current_206(void) {test_case_launch(&test_cases_variable_current[6]);}
void test_variable_current_207(void) {test_case_launch(&test_cases_variable_current[7]);}

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
    RUN_TEST(test_fixed_current_105);
    RUN_TEST(test_fixed_current_106);
    RUN_TEST(test_fixed_current_107);

    // Test cases with variable current values
    printf("\nProtection Overload Test with variable currents (1s delay)\n");
    RUN_TEST(test_variable_current_200);
    RUN_TEST(test_variable_current_201);
    RUN_TEST(test_variable_current_202);
    RUN_TEST(test_variable_current_203);
    RUN_TEST(test_variable_current_204);
    RUN_TEST(test_variable_current_205);
    RUN_TEST(test_variable_current_206);
    RUN_TEST(test_variable_current_207);

    return UNITY_END();    
}
