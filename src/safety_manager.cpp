#include "safety_manager.h"

#include <cmath>

SafetyManager::SafetyManager(const SafetyConfig& config)
    : config_(config), fault_(config.maxSampleGapMs > 0 && config.maxContractionMs > 0
                            ? FaultCode::NONE : FaultCode::INVALID_CONFIGURATION)
{
}

FaultCode SafetyManager::check(uint32_t now)
{
    if (fault_ == FaultCode::NONE)
    {
        if (seenSample_ && uint32_t(now - lastSample_) >= config_.maxSampleGapMs)
            fault_ = FaultCode::STALE_EMG;
        else if (contracted_ && uint32_t(now - contractionSince_) >= config_.maxContractionMs)
            fault_ = FaultCode::ACTIVATION_TIMEOUT;
    }
    return fault_;
}

void SafetyManager::observe(const EMGProcessed& emg, HandCommand command, uint32_t now)
{
    if (check(now) != FaultCode::NONE)
        return; // A late sample cannot erase a timeout.
    if (!emg.valid || !std::isfinite(emg.activation) || emg.activation < 0.0f ||
        emg.activation > 1.0f || command == HandCommand::INVALID)
    {
        fault_ = FaultCode::INVALID_EMG;
        return;
    }
    lastSample_ = now;
    seenSample_ = true;
    if (command == HandCommand::CLOSE && !contracted_)
        contractionSince_ = now;
    contracted_ = command == HandCommand::CLOSE;
}
