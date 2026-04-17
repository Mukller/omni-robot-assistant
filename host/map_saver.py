#!/usr/bin/env python3
"""Save Cartographer map on demand."""
import subprocess, datetime, os

def save_map(output_dir="maps"):
    os.makedirs(output_dir, exist_ok=True)
    ts = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    path = os.path.join(output_dir, f"map_{ts}")
    result = subprocess.run(
        ["ros2", "run", "nav2_map_server", "map_saver_cli",
         "-f", path, "--ros-args", "-p", "save_map_timeout:=5000"],
        capture_output=True, text=True)
    if result.returncode == 0:
        print(f"Map saved: {path}.pgm")
    else:
        print(f"Map save failed: {result.stderr}")

if __name__ == "__main__":
    save_map()
