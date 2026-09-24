# Power budget

The final battery is TBD. An LM2596-based buck converter is planned for the
approximately 5 V servo rail. The exact converter module must be identified
and verified.

The ESP32 power-input path is also TBD. The available DOIT board reference
recommends 7–12 V for external `VIN` operation and does not establish the
approximately 5 V servo rail as a supported `VIN` source. USB may power the
ESP32 during development. The ESP32 3.3 V rail supplies the MyoWare 2.0.
All current figures require measurement or verified part documentation.

| Load | Planned supply | Typical current | Peak/stall current |
| --- | --- | --- | --- |
| ESP32 DOIT DevKit V1 | Input path TBD; USB during development | TBD | TBD |
| MyoWare 2.0 | ESP32 3.3 V | TBD | TBD |
| MG90S #1 | Approximately 5 V | TBD | TBD |
| MG90S #2 | Approximately 5 V | TBD | TBD |
| MG90S #3 | Approximately 5 V | TBD | TBD |
| PCA9685-based controller | Supplies depend on exact breakout | TBD | TBD |

TODO: Record the exact battery, buck module, PCA9685 breakout, supported ESP32
power path, protection/fuse choice, wire and connector ratings, bulk
capacitance, regulator temperature, and measured servo-rail droop during
startup/motion and carefully controlled stall tests.
Size the buck from verified peak load, not average load alone.

## Battery

Final battery: TBD

Initial bench testing may use:
- 6 × AA alkaline cells in series (~9 V nominal), or
- a current-limited laboratory DC supply.

The final battery will be selected after measuring:
- idle current
- single-servo current
- simultaneous three-servo current
- transient/startup current
- worst-case operating current
- desired runtime
