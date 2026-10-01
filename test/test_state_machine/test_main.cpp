#include "command_classifier.h"
#include "emg_processing.h"
#include "safety_manager.h"
#include "servo_controller.h"
#include "state_machine.h"
#include <cassert>
#include <cmath>
#include <limits>

namespace
{
// Fictional bench values, not mechanical limits for this hand.
constexpr ServoMotionConfig motion = {{1100, 1500}, {1700, 1300}, 100, 0.0f};

void near(float actual, float expected)
{
    assert(std::fabs(actual - expected) < 0.01f);
}

void testSignalToPairedTargets()
{
    CommandClassifier classifier({0.6f, 0.3f, 20, 20});
    SafetyManager safety({50, 200});
    HandStateMachine hand;
    ServoController servos(motion);
    assert(servos.begin(0));
    auto sample = [&](float activation, uint32_t time) {
        const EMGProcessed emg = processEMG(20.0f + activation * 100.0f, {20.0f, 120.0f});
        const HandCommand command = classifier.update(emg, time);
        safety.observe(emg, command, time);
        hand.update(command, servos.targetReached(), safety.check(time));
        return servos.update(hand.target(), time, hand.enabled());
    };
    assert(!sample(0.8f, 0).valid);
    assert(!sample(0.05f, 10).valid);
    assert(!sample(0.08f, 20).valid);
    ServoTargets targets = sample(0.11f, 30);
    assert(targets.valid);
    near(targets.wristPulseUs, 1100);
    assert(sample(0.65f, 40).valid);
    targets = sample(0.72f, 60); // Deliberate contraction confirmed.
    assert(hand.state() == HandState::CLOSING);
    near(targets.wristPulseUs, 1180);
    near(targets.thumbPulseUs, 1620); // Reversed mounting, synchronized progress.
    targets = sample(0.42f, 80);
    near(targets.wristPulseUs, 1260);
    sample(0.28f, 90);
    targets = sample(0.08f, 110); // Release reverses mid-motion without jumping.
    assert(hand.state() == HandState::OPENING);
    near(targets.wristPulseUs, 1220);
    sample(0.08f, 140);
    sample(0.08f, 150);
    assert(hand.state() == HandState::OPEN);

    hand.update(HandCommand::OPEN, true, safety.check(200));
    assert(hand.fault() == FaultCode::STALE_EMG);
    assert(!servos.update(hand.target(), 200, hand.enabled()).valid);
    assert(!sample(0.1f, 210).valid); // Later good data cannot recover a fault.
}

void testMotionBoundsAndInitialization()
{
    ServoController servos(motion);
    assert(servos.begin(UINT32_MAX - 19));
    assert(!servos.update(1.0f, UINT32_MAX - 9, false).valid);
    auto targets = servos.update(1.0f, UINT32_MAX - 9, true);
    near(targets.wristPulseUs, 1100); // No accumulated motion while disabled.
    targets = servos.update(1.0f, 10, true);
    near(targets.wristPulseUs, 1180); // 20 ms across rollover.
    targets = servos.update(1.0f, 1000, true);
    near(targets.wristPulseUs, 1500);
    near(targets.thumbPulseUs, 1300);
    assert(servos.targetReached());
    assert(!servos.update(1.1f, 1010, true).valid);
    assert(!servos.update(std::numeric_limits<float>::quiet_NaN(), 1020, true).valid);
    assert(!servos.update(0.0f, 1030, false).valid);

    ServoController defaults({{0, 0}, {0, 0}, 0, -1.0f});
    assert(!defaults.begin(0));
    assert(!defaults.update(1.0f, 100, true).valid);
    auto bad = motion;
    bad.initialPosition = -1.0f;
    assert(!ServoController(bad).begin(0));
    bad = motion;
    bad.fullTravelMs = 0;
    assert(!ServoController(bad).begin(0));
    bad = motion;
    bad.thumb.contractedPulseUs = 0;
    assert(!ServoController(bad).begin(0));
}

void testFaultsAndTransitions()
{
    HandStateMachine hand;
    hand.update(HandCommand::CLOSE, true, FaultCode::NONE);
    assert(hand.state() == HandState::STARTUP);
    hand.update(HandCommand::OPEN, true, FaultCode::NONE);
    assert(hand.state() == HandState::OPENING);
    hand.update(HandCommand::OPEN, true, FaultCode::NONE);
    assert(hand.state() == HandState::OPEN);
    hand.update(HandCommand::CLOSE, true, FaultCode::NONE);
    assert(hand.state() == HandState::CLOSING); // Previous completion is ignored.
    hand.update(HandCommand::CLOSE, false, FaultCode::NONE);
    assert(hand.state() == HandState::CLOSING);
    hand.update(HandCommand::CLOSE, true, FaultCode::NONE);
    assert(hand.state() == HandState::CLOSED);
    hand.update(HandCommand::INVALID, false, FaultCode::NONE);
    assert(hand.fault() == FaultCode::INVALID_EMG);
    hand.update(HandCommand::OPEN, true, FaultCode::NONE);
    assert(!hand.enabled());

    SafetyManager timeout({50, 80});
    timeout.observe({0.8f, true}, HandCommand::CLOSE, UINT32_MAX - 39);
    timeout.observe({0.8f, true}, HandCommand::CLOSE, 0);
    assert(timeout.check(39) == FaultCode::NONE);
    assert(timeout.check(40) == FaultCode::ACTIVATION_TIMEOUT);
    timeout.observe({0.1f, true}, HandCommand::OPEN, 41);
    assert(timeout.check(41) == FaultCode::ACTIVATION_TIMEOUT);

    SafetyManager late({50, 80});
    late.observe({0.1f, true}, HandCommand::OPEN, 0);
    late.observe({0.1f, true}, HandCommand::OPEN, 50);
    assert(late.check(50) == FaultCode::STALE_EMG);

    SafetyManager invalid({50, 80});
    invalid.observe({0.5f, false}, HandCommand::NONE, 0);
    assert(invalid.check(0) == FaultCode::INVALID_EMG);
    SafetyManager unconfigured({0, 0});
    assert(unconfigured.check(0) == FaultCode::INVALID_CONFIGURATION);
}
}

int main()
{
    testSignalToPairedTargets();
    testMotionBoundsAndInitialization();
    testFaultsAndTransitions();
}
