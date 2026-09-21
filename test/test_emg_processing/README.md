# EMG processing tests

The values in `test_main.cpp` are synthetic and are not MyoWare calibration
values or activation thresholds. Run the hardware-independent tests on a host:

```sh
g++ -std=c++11 -Wall -Wextra -Werror -Iinclude src/emg_processing.cpp \
    test/test_emg_processing/test_main.cpp -o /tmp/test_emg_processing
/tmp/test_emg_processing
```

TODO: Characterize real ENV data and decide whether smoothing is needed.
