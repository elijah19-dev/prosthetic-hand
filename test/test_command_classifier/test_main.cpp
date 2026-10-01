#include "command_classifier.h"
#include <cassert>
#include <initializer_list>
#include <limits>

// Synthetic parameters only; not human calibration or recommended timings.
constexpr EMGThresholds thresholds = {0.6f, 0.3f, 20, 30};

int main()
{
    CommandClassifier classifier(thresholds);
    auto sample = [&](float activation, uint32_t time) {
        return classifier.update({activation, true}, time);
    };
    assert(sample(0.8f, 0) == HandCommand::NONE);
    assert(sample(0.8f, 100) == HandCommand::NONE); // No startup contraction.
    assert(sample(0.1f, 110) == HandCommand::NONE);
    assert(sample(0.4f, 130) == HandCommand::NONE); // Interrupted release.
    assert(sample(0.1f, 140) == HandCommand::NONE);
    assert(sample(0.1f, 169) == HandCommand::NONE);
    assert(sample(0.3f, 170) == HandCommand::OPEN);
    assert(sample(0.7f, 180) == HandCommand::OPEN);
    assert(sample(0.4f, 190) == HandCommand::OPEN); // Spike rejected.
    assert(sample(0.6f, 200) == HandCommand::OPEN);
    assert(sample(0.7f, 219) == HandCommand::OPEN);
    assert(sample(0.7f, 220) == HandCommand::CLOSE);
    assert(sample(0.4f, 230) == HandCommand::CLOSE); // Hysteresis band holds.
    assert(sample(0.3f, 240) == HandCommand::CLOSE);
    assert(sample(0.4f, 260) == HandCommand::CLOSE); // Release interrupted.
    assert(sample(0.3f, 270) == HandCommand::CLOSE);
    assert(sample(0.1f, 300) == HandCommand::OPEN);

    assert(classifier.update({0.8f, false}, 310) == HandCommand::INVALID);
    assert(sample(0.8f, 400) == HandCommand::NONE); // Requires release again.
    assert(sample(std::numeric_limits<float>::quiet_NaN(), 410) == HandCommand::INVALID);
    assert(sample(-0.1f, 420) == HandCommand::INVALID);
    assert(sample(1.1f, 430) == HandCommand::INVALID);

    for (const EMGThresholds bad : {EMGThresholds{0.3f, 0.6f, 20, 30},
                                  EMGThresholds{0.6f, 0.3f, 0, 30},
                                  EMGThresholds{1.1f, 0.3f, 20, 30}})
    {
        CommandClassifier invalid(bad);
        assert(invalid.update({0.1f, true}, 0) == HandCommand::INVALID);
    }

    CommandClassifier rollover(thresholds);
    assert(rollover.update({0.1f, true}, UINT32_MAX - 19) == HandCommand::NONE);
    assert(rollover.update({0.1f, true}, 10) == HandCommand::OPEN);
}
