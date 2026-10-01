#include "command_classifier.h"

#include <cmath>

CommandClassifier::CommandClassifier(const EMGThresholds& thresholds)
    : thresholds_(thresholds),
      configured_(std::isfinite(thresholds.on) && std::isfinite(thresholds.off) &&
                  thresholds.off >= 0.0f && thresholds.on <= 1.0f &&
                  thresholds.off < thresholds.on &&
                  thresholds.activationMs > 0 && thresholds.releaseMs > 0)
{
}

HandCommand CommandClassifier::update(const EMGProcessed& emg, uint32_t now)
{
    if (!configured_ || !emg.valid || !std::isfinite(emg.activation) ||
        emg.activation < 0.0f || emg.activation > 1.0f)
    {
        stable_ = pending_ = HandCommand::NONE;
        return HandCommand::INVALID;
    }

    HandCommand candidate = stable_;
    if (emg.activation <= thresholds_.off)
        candidate = HandCommand::OPEN;
    else if (stable_ != HandCommand::NONE && emg.activation >= thresholds_.on)
        candidate = HandCommand::CLOSE;

    if (candidate == stable_)
    {
        pending_ = HandCommand::NONE;
        return stable_;
    }
    if (candidate != pending_)
    {
        pending_ = candidate;
        pendingSince_ = now;
    }
    const uint32_t dwell = candidate == HandCommand::CLOSE
        ? thresholds_.activationMs : thresholds_.releaseMs;
    if (uint32_t(now - pendingSince_) >= dwell)
    {
        stable_ = candidate;
        pending_ = HandCommand::NONE;
    }
    return stable_;
}
