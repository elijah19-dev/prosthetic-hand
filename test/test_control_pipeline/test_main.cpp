#include "command_classifier.h"
#include "state_machine.h"

#include <cassert>
#include <cstdint>

namespace
{
const CommandClassifierConfig syntheticConfig = {0.60f, 0.30f, 100U, 80U};

void applySample(CommandClassifier& classifier,
                 HandStateMachine& machine,
                 float activation,
                 std::uint32_t nowMs,
                 HandCommand expectedCommand,
                 HandTarget expectedTarget)
{
    const HandCommand command = classifier.update({activation, true}, nowMs);
    assert(command == expectedCommand);
    assert(machine.handleCommand(command));
    assert(machine.requestedTarget() == expectedTarget);
}

void testTwoContractionsToggleExactlyTwice()
{
    CommandClassifier classifier(syntheticConfig);
    HandStateMachine machine;
    assert(machine.begin(HandTarget::OPEN));

    applySample(classifier, machine, 0.05f, 0U, HandCommand::NONE, HandTarget::OPEN);
    applySample(classifier, machine, 0.65f, 10U, HandCommand::NONE, HandTarget::OPEN);
    applySample(classifier, machine, 0.72f, 109U, HandCommand::NONE, HandTarget::OPEN);
    applySample(classifier, machine, 0.70f, 110U, HandCommand::TOGGLE,
                HandTarget::CLOSED);

    // Sustained activation does not create additional state transitions.
    applySample(classifier, machine, 0.80f, 500U, HandCommand::NONE,
                HandTarget::CLOSED);

    applySample(classifier, machine, 0.20f, 510U, HandCommand::NONE,
                HandTarget::CLOSED);
    applySample(classifier, machine, 0.20f, 589U, HandCommand::NONE,
                HandTarget::CLOSED);
    applySample(classifier, machine, 0.20f, 590U, HandCommand::NONE,
                HandTarget::CLOSED);

    applySample(classifier, machine, 0.70f, 600U, HandCommand::NONE,
                HandTarget::CLOSED);
    applySample(classifier, machine, 0.70f, 700U, HandCommand::TOGGLE,
                HandTarget::OPEN);
}
} // namespace

int main()
{
    testTwoContractionsToggleExactlyTwice();
}
