# Command classifier tests

The tests use synthetic normalized activation and timing values. They cover
hysteresis, minimum contraction duration, release/rearm timing, sustained
contractions, noise/short pulses, invalid input, invalid configuration, and
`millis()` wraparound. They do not establish user calibration values.

Run the hardware-independent tests on a host:

```sh
g++ -std=c++11 -Wall -Wextra -Werror -Iinclude src/command_classifier.cpp \
    test/test_command_classifier/test_main.cpp -o /tmp/test_command_classifier
/tmp/test_command_classifier
```
