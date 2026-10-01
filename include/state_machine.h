#pragma once

#include "system_types.h"

class HandStateMachine
{
public:
    void update(HandCommand command, bool targetReached, FaultCode fault);
    HandState state() const { return state_; }
    FaultCode fault() const { return fault_; }
    bool enabled() const;
    float target() const;

private:
    HandState state_ = HandState::STARTUP;
    FaultCode fault_ = FaultCode::NONE;
};
