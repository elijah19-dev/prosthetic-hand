#pragma once

#include "system_types.h"
#include <cstdint>

struct ServoEndpoints
{
    uint16_t relaxedPulseUs;
    uint16_t contractedPulseUs; // May be less than relaxed for reversed mounting.
};

struct ServoMotionConfig
{
    ServoEndpoints wrist;
    ServoEndpoints thumb;
    uint32_t fullTravelMs;
    float initialPosition; // Explicit known pose, 0 = relaxed, 1 = contracted.
};

// Target generator only. No PCA9685/PWM output until hardware is characterized.
class ServoController
{
public:
    explicit ServoController(const ServoMotionConfig& config) : config_(config) {}
    bool begin(uint32_t now);
    ServoTargets update(float target, uint32_t now, bool enabled);
    bool targetReached() const { return targets_.valid && position_ == target_; }

private:
    ServoMotionConfig config_;
    bool configured_ = false;
    uint32_t previousTime_ = 0;
    float position_ = 0.0f;
    float target_ = 0.0f;
    ServoTargets targets_ = {0.0f, 0.0f, false};
};
