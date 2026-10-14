#pragma once

#include <cstdint>

// Semantic types shared between firmware layers. No GPIO or driver types belong here.
enum class HandCommand : std::uint8_t
{
    NONE,
    TOGGLE
};

// A target is a request sent toward the actuator layer, not confirmation that
// the mechanism physically reached that position.
enum class HandTarget : std::uint8_t
{
    UNSET,
    OPEN,
    CLOSED
};

enum class HandState : std::uint8_t
{
    STARTUP,
    ACTIVE,
    STOPPED,
    FAULT
};

enum class FaultCode : std::uint8_t
{
    NONE,
    INVALID_EMG,
    SERVO_COMMUNICATION
};

struct EMGProcessed
{
    float activation; // Normalized 0.0-1.0 when valid; calibration is TBD.
    bool valid;
};
