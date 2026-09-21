#include "emg_processing.h"

#include <cassert>
#include <cmath>
#include <limits>

namespace
{
// Synthetic values only: all numbers are in arbitrary matching units.
const EMGCalibrationValues syntheticCalibration = {20.0f, 120.0f};

void expectActivation(float measurement, float expected)
{
    const EMGProcessed result = processEMG(measurement, syntheticCalibration);
    assert(result.valid);
    assert(std::fabs(result.activation - expected) < 0.0001f);
}

void expectInvalid(float measurement, const EMGCalibrationValues& calibration)
{
    const EMGProcessed result = processEMG(measurement, calibration);
    assert(!result.valid);
    assert(result.activation == 0.0f);
}
} // namespace

int main()
{
    expectActivation(20.0f, 0.0f);   // Resting baseline.
    expectActivation(120.0f, 1.0f);  // Contraction reference.
    expectActivation(70.0f, 0.5f);   // Halfway after baseline correction.
    expectActivation(10.0f, 0.0f);   // Below baseline clamps low.
    expectActivation(150.0f, 1.0f);  // Above reference clamps high.

    expectInvalid(70.0f, {20.0f, 20.0f});  // Zero calibration span.
    expectInvalid(70.0f, {120.0f, 20.0f}); // Reversed calibration span.
    expectInvalid(70.0f, {-1.0f, 20.0f});  // Negative ENV baseline.
    expectInvalid(std::numeric_limits<float>::quiet_NaN(), syntheticCalibration);
    expectInvalid(70.0f, {20.0f, std::numeric_limits<float>::infinity()});
}
