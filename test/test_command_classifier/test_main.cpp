#include "command_classifier.h"

#include <cassert>
#include <limits>

namespace
{
// Synthetic values only. These are not calibrated thresholds or timings.
const CommandClassifierConfig syntheticConfig = {0.60f, 0.30f, 100U, 80U};

EMGProcessed sample(float activation)
{
    return {activation, true};
}

void expectNoEvent(CommandClassifier& classifier, float activation, std::uint32_t nowMs)
{
    assert(classifier.update(sample(activation), nowMs) == HandCommand::NONE);
}

void testDeliberateContractionAndSustainedLatch()
{
    CommandClassifier classifier(syntheticConfig);
    assert(classifier.isConfigured());

    expectNoEvent(classifier, 0.05f, 0U);
    expectNoEvent(classifier, 0.65f, 10U);
    expectNoEvent(classifier, 0.72f, 109U);
    assert(classifier.update(sample(0.70f), 110U) == HandCommand::TOGGLE);

    // A held contraction emits only once.
    expectNoEvent(classifier, 0.90f, 500U);
    expectNoEvent(classifier, 0.65f, 900U);
}

void testHysteresisRejectsNoiseAndShortPulses()
{
    CommandClassifier classifier(syntheticConfig);

    expectNoEvent(classifier, 0.59f, 0U);
    expectNoEvent(classifier, 0.61f, 10U);
    expectNoEvent(classifier, 0.45f, 50U); // Between thresholds: remain pending.
    expectNoEvent(classifier, 0.29f, 80U); // Below release: cancel pulse.
    expectNoEvent(classifier, 0.61f, 100U);
    expectNoEvent(classifier, 0.29f, 150U); // Second short pulse also cancels.
    expectNoEvent(classifier, 0.45f, 500U); // Never crossed contraction threshold.
}

void testReleaseMustCompleteBeforeRearm()
{
    CommandClassifier classifier(syntheticConfig);

    expectNoEvent(classifier, 0.70f, 0U);
    assert(classifier.update(sample(0.70f), 100U) == HandCommand::TOGGLE);

    expectNoEvent(classifier, 0.20f, 120U);
    expectNoEvent(classifier, 0.20f, 199U);
    expectNoEvent(classifier, 0.40f, 200U); // Incomplete release is cancelled.
    expectNoEvent(classifier, 0.20f, 210U);
    expectNoEvent(classifier, 0.20f, 290U); // Rearmed after 80 ms release.
    expectNoEvent(classifier, 0.70f, 300U);
    assert(classifier.update(sample(0.70f), 400U) == HandCommand::TOGGLE);
}

void testInvalidInputCannotCreateOrRearmEvent()
{
    CommandClassifier classifier(syntheticConfig);

    expectNoEvent(classifier, 0.70f, 0U);
    assert(classifier.update({0.70f, false}, 100U) == HandCommand::NONE);
    expectNoEvent(classifier, 0.70f, 101U); // Timing restarts after invalid input.
    assert(classifier.update(sample(0.70f), 201U) == HandCommand::TOGGLE);

    expectNoEvent(classifier, 0.20f, 210U);
    assert(classifier.update({0.0f, false}, 290U) == HandCommand::NONE);
    expectNoEvent(classifier, 0.70f, 400U); // Still latched; invalid was not release.
    assert(classifier.update({std::numeric_limits<float>::quiet_NaN(), true}, 500U) ==
           HandCommand::NONE);
    assert(classifier.update(sample(1.1f), 600U) == HandCommand::NONE);
}

void testInvalidConfigurationNeverEmits()
{
    CommandClassifier equalThresholds({0.50f, 0.50f, 0U, 0U});
    assert(!equalThresholds.isConfigured());
    assert(equalThresholds.update(sample(1.0f), 0U) == HandCommand::NONE);

    CommandClassifier outOfRange({1.10f, 0.20f, 0U, 0U});
    assert(!outOfRange.isConfigured());
    assert(outOfRange.update(sample(1.0f), 0U) == HandCommand::NONE);
}

void testMillisWraparound()
{
    CommandClassifier classifier({0.60f, 0.30f, 32U, 0U});
    const std::uint32_t start = std::numeric_limits<std::uint32_t>::max() - 15U;
    expectNoEvent(classifier, 0.70f, start);
    assert(classifier.update(sample(0.70f), 16U) == HandCommand::TOGGLE);
}
} // namespace

int main()
{
    testDeliberateContractionAndSustainedLatch();
    testHysteresisRejectsNoiseAndShortPulses();
    testReleaseMustCompleteBeforeRearm();
    testInvalidInputCannotCreateOrRearmEvent();
    testInvalidConfigurationNeverEmits();
    testMillisWraparound();
}
