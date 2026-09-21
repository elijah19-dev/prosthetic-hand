# Hardware test plan

Tests below require the actual components and an agreed way to remove power.
Record parts, wiring, instruments, observations, and results for each run.

## Acquisition

- Verify GPIO 34 wiring and stable serial output.
- Record relaxed, moderate, and strong contraction raw ADC and millivolt values.
- Check clipping, repeatability, electrode repositioning, and motion artifacts.
- Compare battery-only and USB-connected development setups.

## Power and integration

- Measure buck output unloaded, with ESP32, with one servo, and with three.
- Observe 5 V rail at startup and during simultaneous motion; check resets.
- Observe MyoWare ENV while servos move and with carefully controlled load.
- Verify return routing keeps servo current away from the ADC reference path.

## Servos

- Select the driver and verify one-servo movement before three-servo operation.
- Determine each servo's mapping, travel limits, mechanical end stops, current,
  motion timing, and thermal behavior experimentally.
- Agree on startup, stop, and fault behavior before enabling hand motion.
