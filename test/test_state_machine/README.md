# State machine tests

The tests verify that startup requires an explicit initial target, toggling
works in both directions, repeated command sequences remain deterministic,
invalid enum inputs are rejected, both defined fault codes work from relevant
states, and STOPPED/FAULT states revoke the requested target and gate further
commands. Revocation does not select a mechanical fallback pose. Requested
targets are commands, not confirmed physical positions.

Run the hardware-independent tests on a host:

```sh
g++ -std=c++11 -Wall -Wextra -Werror -Iinclude src/state_machine.cpp \
    test/test_state_machine/test_main.cpp -o /tmp/test_state_machine
/tmp/test_state_machine
```
