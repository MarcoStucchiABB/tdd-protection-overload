// Protection Overload 

#include "protection_overload.h"
#include <math.h>

#define   CALL_RATE 0.01f       // Call rate [s] = 10 ms

// State machine instance
static ProtectionOverloadSM sm;

// Entry a new state
static void ProtectionOverload_SM_EnterState(ProtectionOverloadState state) {  
    // Set new state and entry flag
    sm.state = state;
    sm.entry = true;
}

// State Machine Initialization
void ProtectionOverload_SM_Init(ProtectionOverloadParams *params) {
    // Init SM state
    ProtectionOverload_SM_EnterState(ST_IDLE);

    // Get call rate
    sm.call_rate_sec = ProtectionOverload_SM_GetCallRate();

    // Clear energy storage
    sm.accumulated_energy = 0.0f;

    // Init operating parameters
    sm.params = *params;
}

// Return protection call rate [s]
float ProtectionOverload_SM_GetCallRate(void) {
    return CALL_RATE;
}

// Run state machine (called periodically)
void ProtectionOverload_SM_Run() {

    // Protection State Machine
    switch (sm.state) {

        case ST_IDLE: {

            // Entry function
            if (sm.entry) {
                // Reset entry flag
                sm.entry = false;
            }

            // Read current sensor value
            float maxCurrent = Sensor_Read();
            
            // Compute overload factor: I / I_trip
            float overload_factor = maxCurrent / sm.params.overload_threshold;

            if (overload_factor > 1.15f) {

                // Compute inverse-time trip curve: t_trip = k / ((I/I_trip)^n - 1)
                float trip_time_sec = sm.params.k_factor / (powf(overload_factor, 2)-1);

                // Accumulate energy based on time step
                sm.accumulated_energy += (sm.call_rate_sec / trip_time_sec);

                // Check if accumulated energy exceeds 1.0 (tripping threshold)
                if (sm.accumulated_energy >= 1.0f) {
                    // Trip protection
                    ProtectionOverload_SM_EnterState(ST_OVERLOAD_TRIGGERED);
                }
            } else {
                // If current drops below threshold, slowly reset energy (hysteresis)
                sm.accumulated_energy -= sm.call_rate_sec / sm.params.max_energy;
                if (sm.accumulated_energy < 0.0f) sm.accumulated_energy = 0.0f;
            }
            break;
        }
        
        case ST_OVERLOAD_TRIGGERED:

            // Entry function
            if (sm.entry) {
                // Reset entry flag
                sm.entry = false;
            }
    
            // Once triggered, remain in this state until reset (not implemented here)
            break;
    }
}

/* Returns current state machine state */
ProtectionOverloadState ProtectionOverload_SM_GetState() {
    return sm.state;
}
    
