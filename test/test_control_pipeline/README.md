# Control pipeline integration test

This hardware-independent test passes synthetic normalized EMG samples through
the command classifier and then into the state machine. It verifies that two
deliberate contractions separated by a valid release produce exactly two
target changes, while sustained activation produces no repeated toggle.

All activation thresholds and timings are synthetic fixtures, not calibration
or hardware values.

Run on a host:

```sh
g++ -std=c++11 -Wall -Wextra -Werror -Iinclude \
    src/command_classifier.cpp src/state_machine.cpp \
    test/test_control_pipeline/test_main.cpp -o /tmp/test_control_pipeline
/tmp/test_control_pipeline
```
