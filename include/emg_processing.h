#pragma once

#include "system_types.h"

// Values are supplied by calibration in the same units as measurement
// (for example, raw ADC counts or millivolts). No hardware values are assumed.
struct EMGCalibrationValues
{
    float restingBaseline;
    float contractionReference;
};

// Normalize one ENV measurement without ADC access or internal state.
// Returns {0.0f, false} for nonfinite input, negative baseline, or a
// contraction reference that is not greater than the baseline.
// A valid result is clamped to [0.0f, 1.0f]. No smoothing is applied.
EMGProcessed processEMG(float measurement, const EMGCalibrationValues& calibration);
