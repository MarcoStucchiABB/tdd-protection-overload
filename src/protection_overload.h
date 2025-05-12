// Protection Overload Header

#pragma once

#include <stdbool.h>

// State Machine States
typedef enum {
    ST_IDLE,                            // Protection active and running
    ST_OVERLOAD_TRIGGERED               // Protection triggered Breaker opening 
} ProtectionOverloadState;

// Parameters Structure
typedef struct {
    float overload_threshold;           // Current threshold
    float k_factor;                     // IEC 60947 protection k
    float cooling_rate;
    float max_energy;
} ProtectionOverloadParams;

// State Machine parameters
typedef struct {
    ProtectionOverloadState state;      // Current state
    bool entry;                         // State entry flag
    float call_rate_sec;                // Call rate [s]
    ProtectionOverloadParams params;    // Operating parameters
    float accumulated_energy;           // Energy accumulator
} ProtectionOverloadSM;

// API Functions
void ProtectionOverload_SM_Init(ProtectionOverloadParams *params);
float ProtectionOverload_SM_GetCallRate();
void ProtectionOverload_SM_Run();
ProtectionOverloadState ProtectionOverload_SM_GetState();

// Sensor input function (mocked in tests)
float Sensor_Read();