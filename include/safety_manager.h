#pragma once

#include "system_types.h"

#include <cstdint>

struct SafetyConfig
{
    uint32_t maxSampleGapMs;
    uint32_t maxContractionMs;
};

// Software watchdog only; cannot detect a disconnected sensor at a plausible
// voltage or a stalled motor. Physical stop/OE/power behavior remains TBD.
class SafetyManager
{
public:
    explicit SafetyManager(const SafetyConfig& config);
    void observe(const EMGProcessed& emg, HandCommand command, uint32_t now);
    FaultCode check(uint32_t now);

private:
    SafetyConfig config_;
    FaultCode fault_;
    bool seenSample_ = false;
    bool contracted_ = false;
    uint32_t lastSample_ = 0;
    uint32_t contractionSince_ = 0;
};
