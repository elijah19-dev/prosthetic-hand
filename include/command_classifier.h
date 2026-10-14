#pragma once

#include <cstdint>

#include "system_types.h"

// All values are supplied by calibration/configuration. This module does not
// establish hardware thresholds or timing policy.
struct CommandClassifierConfig
{
    float contractionThreshold;
    float releaseThreshold;
    std::uint32_t minimumContractionMs;
    std::uint32_t minimumReleaseMs;
};

class CommandClassifier
{
public:
    explicit CommandClassifier(const CommandClassifierConfig& config);

    // Emits TOGGLE once per deliberate contraction. A new event cannot be
    // emitted until a valid release has satisfied minimumReleaseMs.
    HandCommand update(const EMGProcessed& emg, std::uint32_t nowMs);

    void reset();
    bool isConfigured() const;

private:
    enum class Phase : std::uint8_t
    {
        ARMED,
        CONTRACTION_PENDING,
        LATCHED,
        RELEASE_PENDING
    };

    bool isInputValid(const EMGProcessed& emg) const;
    static bool elapsed(std::uint32_t nowMs,
                        std::uint32_t startMs,
                        std::uint32_t durationMs);

    CommandClassifierConfig config_;
    Phase phase_;
    std::uint32_t phaseStartMs_;
    bool configured_;
};
