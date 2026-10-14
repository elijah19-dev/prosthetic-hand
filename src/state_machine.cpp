#include "state_machine.h"

HandStateMachine::HandStateMachine()
    : state_(HandState::STARTUP),
      requestedTarget_(HandTarget::UNSET),
      faultCode_(FaultCode::NONE)
{
}

bool HandStateMachine::begin(HandTarget initialTarget)
{
    if (state_ != HandState::STARTUP || !isActionableTarget(initialTarget))
    {
        return false;
    }

    requestedTarget_ = initialTarget;
    state_ = HandState::ACTIVE;
    return true;
}

bool HandStateMachine::handleCommand(HandCommand command)
{
    if (state_ != HandState::ACTIVE)
    {
        return false;
    }

    switch (command)
    {
    case HandCommand::NONE:
        return true;

    case HandCommand::TOGGLE:
        if (requestedTarget_ == HandTarget::OPEN)
        {
            requestedTarget_ = HandTarget::CLOSED;
            return true;
        }
        if (requestedTarget_ == HandTarget::CLOSED)
        {
            requestedTarget_ = HandTarget::OPEN;
            return true;
        }
        return false;
    }

    return false;
}

void HandStateMachine::stop()
{
    if (state_ != HandState::FAULT)
    {
        requestedTarget_ = HandTarget::UNSET;
        state_ = HandState::STOPPED;
    }
}

bool HandStateMachine::enterFault(FaultCode fault)
{
    switch (fault)
    {
    case FaultCode::INVALID_EMG:
    case FaultCode::SERVO_COMMUNICATION:
        requestedTarget_ = HandTarget::UNSET;
        state_ = HandState::FAULT;
        faultCode_ = fault;
        return true;

    case FaultCode::NONE:
        return false;
    }

    return false;
}

HandState HandStateMachine::state() const
{
    return state_;
}

HandTarget HandStateMachine::requestedTarget() const
{
    return requestedTarget_;
}

FaultCode HandStateMachine::faultCode() const
{
    return faultCode_;
}

bool HandStateMachine::isActionableTarget(HandTarget target)
{
    return target == HandTarget::OPEN || target == HandTarget::CLOSED;
}
