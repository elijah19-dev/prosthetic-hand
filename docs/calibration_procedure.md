# EMG calibration procedure

Status: normalization is implemented; measured calibration and automatic
collection remain TBD. Supply raw-ADC baseline and contraction reference in
`EMG_CALIBRATION` in `include/config.h`; do not use the synthetic test values.

Record electrode placement and MyoWare ENV readings at rest, during repeated
controlled contractions, and after repositioning. Capture baseline variation,
contraction range, clipping, motion artifacts, and any servo-related noise.

TODO: Establish the collection duration, acceptance criteria, normalization
parameters, and recalibration triggers from measured data. Do not choose
activation thresholds from the scaffold.

The paired wrist/thumb controller also needs separate on/off thresholds,
activation/release durations, watchdog limits, measured pulse endpoints, full
travel time and an explicitly known initial pose. See
[control setup](wrist_thumb_control.md). Rebuild/reset after applying a measured
session configuration; a valid sample alone does not clear a latched fault.
