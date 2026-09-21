#include "emg_processing.h"

#include <cmath>

EMGProcessed processEMG(float measurement, const EMGCalibrationValues& calibration)
{
    if (!std::isfinite(measurement) ||
        !std::isfinite(calibration.restingBaseline) ||
        !std::isfinite(calibration.contractionReference) ||
        calibration.restingBaseline < 0.0f ||
        calibration.contractionReference <= calibration.restingBaseline)
    {
        return {0.0f, false};
    }

    // Compare before subtracting so extreme finite inputs still clamp cleanly.
    if (measurement <= calibration.restingBaseline)
    {
        return {0.0f, true};
    }
    if (measurement >= calibration.contractionReference)
    {
        return {1.0f, true};
    }

    const float span = calibration.contractionReference - calibration.restingBaseline;
    return {(measurement - calibration.restingBaseline) / span, true};
}
