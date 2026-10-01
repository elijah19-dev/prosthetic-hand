#include "servo_controller.h"
#include "config.h"

#include <algorithm>
#include <cmath>

namespace
{
bool validEndpoints(const ServoEndpoints& endpoints)
{
    // The documented 50 Hz servo period is 20000 us, not a travel limit.
    return endpoints.relaxedPulseUs > 0 && endpoints.contractedPulseUs > 0 &&
           endpoints.relaxedPulseUs < SERVO_PWM_PERIOD_US &&
           endpoints.contractedPulseUs < SERVO_PWM_PERIOD_US &&
           endpoints.relaxedPulseUs != endpoints.contractedPulseUs;
}

float pulseAt(const ServoEndpoints& endpoints, float position)
{
    return endpoints.relaxedPulseUs + position *
        (float(endpoints.contractedPulseUs) - endpoints.relaxedPulseUs);
}
}

bool ServoController::begin(uint32_t now)
{
    configured_ = validEndpoints(config_.wrist) && validEndpoints(config_.thumb) &&
                  config_.fullTravelMs > 0 && std::isfinite(config_.initialPosition) &&
                  config_.initialPosition >= 0.0f && config_.initialPosition <= 1.0f;
    previousTime_ = now;
    position_ = config_.initialPosition;
    targets_ = {0.0f, 0.0f, false};
    return configured_;
}

ServoTargets ServoController::update(float target, uint32_t now, bool enabled)
{
    const uint32_t elapsed = now - previousTime_;
    previousTime_ = now;
    if (!configured_ || !enabled || !std::isfinite(target) || target < 0.0f || target > 1.0f)
    {
        targets_ = {0.0f, 0.0f, false};
        return targets_;
    }
    // First target uses the supplied initial pose; time spent disabled adds no motion.
    const float step = targets_.valid ? float(elapsed) / config_.fullTravelMs : 0.0f;
    target_ = target;
    position_ += std::max(-step, std::min(step, target - position_));
    position_ = std::max(0.0f, std::min(1.0f, position_));
    targets_ = {pulseAt(config_.wrist, position_), pulseAt(config_.thumb, position_), true};
    return targets_;
}
