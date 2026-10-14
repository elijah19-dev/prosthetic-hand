#include "command_classifier.h"

#include <cmath>

namespace
{
bool validConfig(const CommandClassifierConfig& config)
{
    return std::isfinite(config.contractionThreshold) &&
           std::isfinite(config.releaseThreshold) &&
           config.contractionThreshold >= 0.0f &&
           config.contractionThreshold <= 1.0f &&
           config.releaseThreshold >= 0.0f &&
           config.releaseThreshold < config.contractionThreshold;
}
} // namespace

CommandClassifier::CommandClassifier(const CommandClassifierConfig& config)
    : config_(config),
      phase_(Phase::ARMED),
      phaseStartMs_(0U),
      configured_(validConfig(config))
{
}

HandCommand CommandClassifier::update(const EMGProcessed& emg, std::uint32_t nowMs)
{
    if (!configured_ || !isInputValid(emg))
    {
        // Invalid data never counts as activation or release. Cancel a partial
        // interval while preserving the latch after an emitted event.
        if (phase_ == Phase::CONTRACTION_PENDING)
        {
            phase_ = Phase::ARMED;
        }
        else if (phase_ == Phase::RELEASE_PENDING)
        {
            phase_ = Phase::LATCHED;
        }
        return HandCommand::NONE;
    }

    switch (phase_)
    {
    case Phase::ARMED:
        if (emg.activation >= config_.contractionThreshold)
        {
            if (config_.minimumContractionMs == 0U)
            {
                phase_ = Phase::LATCHED;
                return HandCommand::TOGGLE;
            }
            phase_ = Phase::CONTRACTION_PENDING;
            phaseStartMs_ = nowMs;
        }
        break;

    case Phase::CONTRACTION_PENDING:
        if (emg.activation < config_.releaseThreshold)
        {
            phase_ = Phase::ARMED;
        }
        else if (elapsed(nowMs, phaseStartMs_, config_.minimumContractionMs))
        {
            phase_ = Phase::LATCHED;
            return HandCommand::TOGGLE;
        }
        break;

    case Phase::LATCHED:
        if (emg.activation <= config_.releaseThreshold)
        {
            if (config_.minimumReleaseMs == 0U)
            {
                phase_ = Phase::ARMED;
            }
            else
            {
                phase_ = Phase::RELEASE_PENDING;
                phaseStartMs_ = nowMs;
            }
        }
        break;

    case Phase::RELEASE_PENDING:
        if (emg.activation > config_.releaseThreshold)
        {
            phase_ = Phase::LATCHED;
        }
        else if (elapsed(nowMs, phaseStartMs_, config_.minimumReleaseMs))
        {
            phase_ = Phase::ARMED;
        }
        break;
    }

    return HandCommand::NONE;
}

void CommandClassifier::reset()
{
    phase_ = Phase::ARMED;
    phaseStartMs_ = 0U;
}

bool CommandClassifier::isConfigured() const
{
    return configured_;
}

bool CommandClassifier::isInputValid(const EMGProcessed& emg) const
{
    return emg.valid && std::isfinite(emg.activation) &&
           emg.activation >= 0.0f && emg.activation <= 1.0f;
}

bool CommandClassifier::elapsed(std::uint32_t nowMs,
                                std::uint32_t startMs,
                                std::uint32_t durationMs)
{
    // Unsigned subtraction keeps normal millis() wraparound behavior.
    return static_cast<std::uint32_t>(nowMs - startMs) >= durationMs;
}
