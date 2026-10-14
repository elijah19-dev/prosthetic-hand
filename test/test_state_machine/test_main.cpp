#include "state_machine.h"

#include <cassert>
#include <cstdint>

namespace
{
void testStartupRequiresExplicitTarget()
{
    HandStateMachine machine;
    assert(machine.state() == HandState::STARTUP);
    assert(machine.requestedTarget() == HandTarget::UNSET);
    assert(machine.faultCode() == FaultCode::NONE);
    assert(!machine.handleCommand(HandCommand::NONE));
    assert(!machine.handleCommand(HandCommand::TOGGLE));
    assert(!machine.begin(HandTarget::UNSET));
    assert(machine.begin(HandTarget::OPEN));
    assert(machine.state() == HandState::ACTIVE);
    assert(machine.requestedTarget() == HandTarget::OPEN);
    assert(!machine.begin(HandTarget::CLOSED));
}

void testBothToggleDirections()
{
    HandStateMachine machine;
    assert(machine.begin(HandTarget::OPEN));
    assert(machine.handleCommand(HandCommand::TOGGLE));
    assert(machine.requestedTarget() == HandTarget::CLOSED);
    assert(machine.handleCommand(HandCommand::TOGGLE));
    assert(machine.requestedTarget() == HandTarget::OPEN);
    assert(machine.handleCommand(HandCommand::NONE));
    assert(machine.requestedTarget() == HandTarget::OPEN);
}

void testRepeatedCommandsRemainDeterministic()
{
    HandStateMachine machine;
    assert(machine.begin(HandTarget::CLOSED));

    for (unsigned int index = 0U; index < 100U; ++index)
    {
        assert(machine.handleCommand(HandCommand::NONE));
        assert(machine.requestedTarget() == HandTarget::CLOSED);
    }

    assert(machine.handleCommand(HandCommand::TOGGLE));
    assert(machine.requestedTarget() == HandTarget::OPEN);
    assert(!machine.handleCommand(static_cast<HandCommand>(255U)));
    assert(machine.requestedTarget() == HandTarget::OPEN);
    assert(machine.handleCommand(HandCommand::TOGGLE));
    assert(machine.requestedTarget() == HandTarget::CLOSED);
    assert(machine.handleCommand(HandCommand::TOGGLE));
    assert(machine.requestedTarget() == HandTarget::OPEN);
}

void testInvalidInputsAreRejected()
{
    HandStateMachine machine;
    assert(!machine.begin(static_cast<HandTarget>(255U)));
    assert(machine.begin(HandTarget::CLOSED));
    assert(!machine.handleCommand(static_cast<HandCommand>(255U)));
    assert(machine.requestedTarget() == HandTarget::CLOSED);
    assert(!machine.enterFault(FaultCode::NONE));
    assert(!machine.enterFault(static_cast<FaultCode>(255U)));
    assert(machine.state() == HandState::ACTIVE);
}

void testStopGatesCommandsAndRevokesTarget()
{
    HandStateMachine machine;
    assert(machine.begin(HandTarget::OPEN));
    machine.stop();
    assert(machine.state() == HandState::STOPPED);
    assert(!machine.handleCommand(HandCommand::TOGGLE));
    assert(machine.requestedTarget() == HandTarget::UNSET);
    assert(!machine.begin(HandTarget::CLOSED));

    // Repeated stop calls are idempotent.
    machine.stop();
    assert(machine.state() == HandState::STOPPED);
    assert(machine.requestedTarget() == HandTarget::UNSET);
}

void testStopFromStartupIsTerminal()
{
    HandStateMachine machine;
    machine.stop();
    assert(machine.state() == HandState::STOPPED);
    assert(machine.requestedTarget() == HandTarget::UNSET);
    assert(!machine.begin(HandTarget::OPEN));
    assert(!machine.handleCommand(HandCommand::NONE));
    assert(!machine.handleCommand(HandCommand::TOGGLE));
}

void testFaultGatesCommandsAndCannotBeClearedByStop()
{
    HandStateMachine machine;
    assert(machine.begin(HandTarget::CLOSED));
    assert(machine.enterFault(FaultCode::INVALID_EMG));
    assert(machine.state() == HandState::FAULT);
    assert(machine.faultCode() == FaultCode::INVALID_EMG);
    assert(machine.requestedTarget() == HandTarget::UNSET);
    assert(!machine.handleCommand(HandCommand::TOGGLE));
    assert(machine.requestedTarget() == HandTarget::UNSET);

    machine.stop();
    assert(machine.state() == HandState::FAULT);
    assert(machine.faultCode() == FaultCode::INVALID_EMG);
}

void expectFaultFromStartup(FaultCode fault)
{
    HandStateMachine machine;
    assert(machine.enterFault(fault));
    assert(machine.state() == HandState::FAULT);
    assert(machine.faultCode() == fault);
    assert(machine.requestedTarget() == HandTarget::UNSET);
    assert(!machine.begin(HandTarget::OPEN));
    assert(!machine.handleCommand(HandCommand::NONE));
    assert(!machine.handleCommand(HandCommand::TOGGLE));
}

void expectFaultFromActive(FaultCode fault)
{
    HandStateMachine machine;
    assert(machine.begin(HandTarget::OPEN));
    assert(machine.enterFault(fault));
    assert(machine.state() == HandState::FAULT);
    assert(machine.faultCode() == fault);
    assert(machine.requestedTarget() == HandTarget::UNSET);
    assert(!machine.begin(HandTarget::CLOSED));
    assert(!machine.handleCommand(HandCommand::TOGGLE));
}

void testEveryFaultCodeFromStartupAndActive()
{
    expectFaultFromStartup(FaultCode::INVALID_EMG);
    expectFaultFromStartup(FaultCode::SERVO_COMMUNICATION);
    expectFaultFromActive(FaultCode::INVALID_EMG);
    expectFaultFromActive(FaultCode::SERVO_COMMUNICATION);
}

void testFaultCanEscalateStoppedState()
{
    HandStateMachine machine;
    assert(machine.begin(HandTarget::CLOSED));
    machine.stop();
    assert(machine.enterFault(FaultCode::SERVO_COMMUNICATION));
    assert(machine.state() == HandState::FAULT);
    assert(machine.faultCode() == FaultCode::SERVO_COMMUNICATION);
    assert(machine.requestedTarget() == HandTarget::UNSET);
    assert(!machine.handleCommand(HandCommand::TOGGLE));
}
} // namespace

int main()
{
    testStartupRequiresExplicitTarget();
    testBothToggleDirections();
    testRepeatedCommandsRemainDeterministic();
    testInvalidInputsAreRejected();
    testStopGatesCommandsAndRevokesTarget();
    testStopFromStartupIsTerminal();
    testFaultGatesCommandsAndCannotBeClearedByStop();
    testEveryFaultCodeFromStartupAndActive();
    testFaultCanEscalateStoppedState();
}
