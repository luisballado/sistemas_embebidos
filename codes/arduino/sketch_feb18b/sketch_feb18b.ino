/*
 * RP LIDAR A1 Basic Example
 * 
 * This sketch sends the "start scan" command to the RP LIDAR A1 and then
 * reads measurement packets from the sensor. It prints the quality,
 * angle, and distance values to the Serial Monitor.
 *
 * Wiring:  
 *   - Connect the RP LIDAR A1’s TX/RX (or via an appropriate interface)
 *     to the Arduino’s Serial1 (or SoftwareSerial if using an Uno).
 *
 * IMPORTANT: This is a simplified example. For a robust implementation,
 * proper packet synchronization and error checking are required.
 */

#define RPLIDAR_BAUDRATE 115200

void setup() {
  // Initialize the debugging serial port (Serial Monitor)
  Serial.begin(115200);
  while (!Serial) { ; } // wait for Serial to be ready (useful on Leonardo/Micro)

  // Initialize the serial port for the RP LIDAR A1.
  // On an Arduino Mega, Serial1 is available.
  Serial1.begin(RPLIDAR_BAUDRATE);
  
  delay(1000); // Allow time for the sensor to boot up
  
  // Send the "start scan" command to the LIDAR.
  // According to the datasheet, the command is 0xA5 followed by 0x20.
  Serial1.write(0xA5);
  Serial1.write(0x20);
  
  Serial.println("Sent start scan command to RP LIDAR A1.");
}

void loop() {
  // Check if at least 5 bytes are available.
  // (Many implementations state that each measurement packet is 5 bytes long.)
  if (Serial1.available() >= 5) {
    uint8_t packet[5];
    Serial1.readBytes(packet, 5);

    // Simplified packet parsing:
    // Assumed packet structure (5 bytes):
    // Byte 0: [Quality (upper 7 bits)] + [Start flag (LSB)]
    // Byte 1: Angle (low byte)
    // Byte 2: Angle (high byte)
    // Byte 3: Distance (low byte)
    // Byte 4: Distance (high byte)
    
    uint8_t quality = packet[0] >> 1;   // Quality value (7 bits)
    bool    startFlag = packet[0] & 0x01; // Start flag (should be 1 for the first packet)
    
    // Combine the two angle bytes.
    // Many documents state that the angle is sent as a 16-bit value in 0.01° units.
    uint16_t angleRaw = ((uint16_t)packet[2] << 8) | packet[1];
    float angle = angleRaw / 100.0; // Convert to degrees
    
    // Combine the two distance bytes.
    // Distance is typically in millimeters.
    uint16_t distance = ((uint16_t)packet[4] << 8) | packet[3];
    
    // Print the parsed values to the Serial Monitor.
    Serial.print("Quality: ");
    Serial.print(quality);
    Serial.print(" | Angle: ");
    Serial.print(angle);
    Serial.print("° | Distance: ");
    Serial.print(distance);
    Serial.println(" mm");
  }
}
