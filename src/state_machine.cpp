#include "state_machine.h"

void HandStateMachine::update(HandCommand command, bool targetReached, FaultCode fault)
{
    if (state_ == HandState::FAULT)
        return; // Latched until controller reset; valid input alone cannot recover.
    if (fault != FaultCode::NONE || command == HandCommand::INVALID)
    {
        fault_ = fault != FaultCode::NONE ? fault : FaultCode::INVALID_EMG;
        state_ = HandState::FAULT;
        return;
    }
    if (state_ == HandState::STARTUP && command != HandCommand::OPEN)
        return;
    if (command == HandCommand::OPEN &&
        state_ != HandState::OPEN && state_ != HandState::OPENING)
    {
        state_ = HandState::OPENING;
        return;
    }
    if (command == HandCommand::CLOSE &&
        state_ != HandState::CLOSED && state_ != HandState::CLOSING)
    {
        state_ = HandState::CLOSING;
        return;
    }
    if (targetReached && state_ == HandState::OPENING)
        state_ = HandState::OPEN;
    else if (targetReached && state_ == HandState::CLOSING)
        state_ = HandState::CLOSED;
}

bool HandStateMachine::enabled() const
{
    return state_ == HandState::OPEN || state_ == HandState::OPENING ||
           state_ == HandState::CLOSED || state_ == HandState::CLOSING;
}

float HandStateMachine::target() const
{
    return state_ == HandState::CLOSED || state_ == HandState::CLOSING ? 1.0f : 0.0f;
}
