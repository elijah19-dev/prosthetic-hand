# Project tools

`python tools/run_tests.py` builds and runs all hardware-independent C++ suites
with g++/clang++ on PATH. It also supports the project-local Zig validation
compiler at `.pio/validation-tools/ziglang/zig.exe`. Output stays under `.pio`.
