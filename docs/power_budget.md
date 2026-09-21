# Power budget

The planned 12 V battery feeds an approximately 5 V buck converter. The 5 V
rail supplies the ESP32 VIN/5V input and the servo power rail. The ESP32 3.3 V
rail supplies the MyoWare 2.0. All current figures require measurement or
verified part documentation.

| Load | Planned supply | Typical current | Peak/stall current |
| --- | --- | --- | --- |
| ESP32 DOIT DevKit V1 | 5 V input | TBD | TBD |
| MyoWare 2.0 | ESP32 3.3 V | TBD | TBD |
| MG90S #1 | Approximately 5 V | TBD | TBD |
| MG90S #2 | Approximately 5 V | TBD | TBD |
| MG90S #3 | Approximately 5 V | TBD | TBD |
| External servo driver | TBD | TBD | TBD |

TODO: Record exact battery and buck parts, protection/fuse choice, wire and
connector ratings, bulk capacitance, regulator temperature, and measured 5 V
droop during servo startup/motion and carefully controlled stall tests.
Size the buck from verified peak load, not average load alone.
