import sys
from rplidar import RPLidar

# Change this to your device path
PORT_NAME = "/dev/ttyUSB0"

def run_lidar():
    lidar = RPLidar(PORT_NAME)
    try:
        print("Starting RPLidar A1...")
        for i, scan in enumerate(lidar.iter_scans()):
            print(f"Scan {i}:")
            for quality, angle, distance in scan:
                print(f"  Angle: {angle:.2f}°, Distance: {distance:.2f} mm, Quality: {quality}")
            if i >= 10:  # Stop after 10 scans
                break
    except KeyboardInterrupt:
        print("Stopping...")
    finally:
        lidar.stop()
        lidar.disconnect()
        print("RPLidar disconnected.")

if __name__ == "__main__":
    run_lidar()
