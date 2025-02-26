const int pinA = 35;  // Connect Channel A of the encoder
const int pinB = 34;  // Connect Channel B of the encoder

volatile long tickCount = 0;
volatile int lastEncoded = 0;

void IRAM_ATTR updateEncoder() {
  int MSB = digitalRead(pinA);  // Read Channel A
  int LSB = digitalRead(pinB);  // Read Channel B
  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  // Determine direction based on state transition
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
    tickCount++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
    tickCount--;

  lastEncoded = encoded;
}

void setup() {
  Serial.begin(115200);
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinA), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinB), updateEncoder, CHANGE);
}

void loop() {
  Serial.print("Tick Count: ");
  Serial.println(tickCount);
  delay(1000);  // Adjust delay as needed
}
