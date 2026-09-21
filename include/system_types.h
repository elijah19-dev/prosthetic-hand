#pragma once

// Semantic types shared between firmware layers. No GPIO or driver types belong here.
enum class HandCommand { NONE, OPEN, CLOSE };
enum class HandState { STARTUP, OPEN, OPENING, CLOSED, CLOSING, STOPPED, FAULT };
enum class FaultCode { NONE, INVALID_EMG, SERVO_COMMUNICATION };

struct EMGProcessed
{
    float activation; // Normalized 0.0-1.0 when valid; calibration is TBD.
    bool valid;
};
