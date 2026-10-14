#pragma once

#include "system_types.h"

class HandStateMachine
{
public:
    HandStateMachine();

    // The caller must explicitly select the initial target. OPEN and CLOSED
    // are accepted; UNSET is rejected. No startup pose is assumed here.
    bool begin(HandTarget initialTarget);

    // Returns false when the command is invalid or gated by the current state.
    // NONE is a valid no-op while ACTIVE.
    bool handleCommand(HandCommand command);

    // STOPPED and FAULT revoke the target instead of selecting a mechanical
    // fallback pose. Recovery policy remains intentionally undefined.
    void stop();
    bool enterFault(FaultCode fault);

    HandState state() const;
    HandTarget requestedTarget() const;
    FaultCode faultCode() const;

private:
    static bool isActionableTarget(HandTarget target);

    HandState state_;
    HandTarget requestedTarget_;
    FaultCode faultCode_;
};
