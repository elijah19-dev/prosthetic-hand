# Servo-controller host tests

These tests replace Arduino `Wire` with a host mock and verify that the
production `ServoController` uses the configured SDA/SCL pins, standard-mode
frequency, and expected PCA9685 address. They also verify handling of bus
initialization failure, address NACK, and other bus errors.

They do not prove that a physical PCA9685 board acknowledges.

```sh
g++ -std=c++11 -Wall -Wextra -Werror \
    -Itest/test_servo_controller_host/mocks -Iinclude \
    src/servo_controller.cpp test/test_servo_controller_host/test_main.cpp \
    -o /tmp/test_servo_controller_host
/tmp/test_servo_controller_host
```
