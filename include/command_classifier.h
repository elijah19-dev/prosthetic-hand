#pragma once

#include "system_types.h"

#include <cstdint>

struct EMGThresholds
{
    float on;
    float off;
    uint32_t activationMs;
    uint32_t releaseMs;
};

// Stable level commands, not one-shot events. Startup requires release first.
class CommandClassifier
{
public:
    explicit CommandClassifier(const EMGThresholds& thresholds);
    HandCommand update(const EMGProcessed& emg, uint32_t now);

private:
    EMGThresholds thresholds_;
    bool configured_;
    HandCommand stable_ = HandCommand::NONE;
    HandCommand pending_ = HandCommand::NONE;
    uint32_t pendingSince_ = 0;
};
