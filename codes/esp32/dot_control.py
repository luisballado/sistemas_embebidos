import serial

# Change 'COM5' to match your ESP32 port (e.g., '/dev/ttyUSB0' for Linux/Mac)
SERIAL_PORT = "/dev/ttyUSB0"
BAUD_RATE = 9600

# Open Serial Connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

print("Listening for joystick data... (Press Ctrl+C to stop)")

try:
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode().strip()
            if line.startswith("x = "):  # Ensure it's a valid data line
                try:
                    parts = line.split(", ")
                    x_val = int(parts[0].split(" = ")[1])
                    y_val = int(parts[1].split(" = ")[1])
                    print(f"X: {x_val}, Y: {y_val}")
                except Exception as e:
                    print(f"Error parsing data: {e}")
except KeyboardInterrupt:
    print("\nStopped by user.")
finally:
    ser.close()
