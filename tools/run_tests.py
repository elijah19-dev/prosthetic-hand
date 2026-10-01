"""Run the hardware-independent C++ tests; requires g++, clang++, or local Zig."""
import os
from pathlib import Path
import shutil
import subprocess

root = Path(__file__).resolve().parents[1]
output = root / ".pio" / "host-tests"
output.mkdir(parents=True, exist_ok=True)
compiler = shutil.which("g++") or shutil.which("clang++")
zig = root / ".pio" / "validation-tools" / "ziglang" / "zig.exe"
if compiler:
    command = [compiler]
elif zig.is_file():
    command = [str(zig), "c++"]
else:
    raise SystemExit("Install g++ or clang++ and add it to PATH to run host tests.")

env = os.environ.copy()
env["ZIG_GLOBAL_CACHE_DIR"] = str(output / "zig-cache")
sources = [str(path) for path in sorted((root / "src").glob("*.cpp"))
           if path.name not in {"main.cpp", "emg_sensor.cpp"}]
for test in sorted((root / "test").glob("test_*/test_main.cpp")):
    executable = output / (test.parent.name + (".exe" if os.name == "nt" else ""))
    subprocess.run(command + ["-std=c++11", "-Wall", "-Wextra", "-Werror",
                             "-I" + str(root / "include"), *sources, str(test),
                             "-o", str(executable)], check=True, env=env, cwd=root)
    subprocess.run([str(executable)], check=True, cwd=root)
    print(f"PASS {test.parent.name}", flush=True)
