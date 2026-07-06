#!/usr/bin/env python3
"""Save Cartographer map to disk.

Usage:
  # Save once:
  python3 map_saver.py

  # Save every 60 seconds (useful during mapping sessions):
  python3 map_saver.py --interval 60

  # Custom output directory and filename prefix:
  python3 map_saver.py --dir ~/maps --prefix living_room --interval 30
"""
import argparse
import datetime
import os
import signal
import subprocess
import sys
import time


def save_map(output_dir: str, prefix: str) -> str | None:
    os.makedirs(output_dir, exist_ok=True)
    ts = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    path = os.path.join(output_dir, f"{prefix}_{ts}")

    result = subprocess.run(
        [
            "ros2", "run", "nav2_map_server", "map_saver_cli",
            "-f", path,
            "--ros-args", "-p", "save_map_timeout:=5000",
        ],
        capture_output=True,
        text=True,
        timeout=10,
    )

    if result.returncode == 0:
        print(f"[{_now()}] Map saved: {path}.pgm + {path}.yaml")
        return path
    else:
        print(f"[{_now()}] Save FAILED: {result.stderr.strip()}", file=sys.stderr)
        return None


def _now() -> str:
    return datetime.datetime.now().strftime("%H:%M:%S")


def main() -> None:
    parser = argparse.ArgumentParser(description="Save ROS2/Nav2 map to disk")
    parser.add_argument("--dir", default="maps", help="Output directory (default: maps/)")
    parser.add_argument("--prefix", default="map", help="Filename prefix (default: map)")
    parser.add_argument(
        "--interval", type=int, default=0,
        help="Auto-save interval in seconds (0 = save once and exit)",
    )
    args = parser.parse_args()

    if args.interval <= 0:
        path = save_map(args.dir, args.prefix)
        sys.exit(0 if path else 1)

    print(f"Auto-saving every {args.interval}s to {args.dir}/. Press Ctrl+C to stop.")
    signal.signal(signal.SIGINT, lambda *_: (print("\nStopped."), sys.exit(0)))
    while True:
        save_map(args.dir, args.prefix)
        time.sleep(args.interval)


if __name__ == "__main__":
    main()
