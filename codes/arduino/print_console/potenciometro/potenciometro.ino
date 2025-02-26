float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// the setup routine runs once when you press reset:
void setup() {
  pinMode(15, OUTPUT);  // sets the pin as output
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // set the ADC attenuation to 11 dB (up to ~3.3V input)
  analogSetAttenuation(ADC_11db);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin GPIO36:
  int analogValue = analogRead(14);
  // Rescale to potentiometer's voltage (from 0V to 3.3V):
  float voltage = floatMap(analogValue, 0, 4095, 0, 3.3);

  // print out the value you read:
  Serial.print("Analog: ");
  Serial.print(analogValue);
  Serial.print(", Voltage: ");
  Serial.println(voltage);
  analogWrite(15, analogValue / 4); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255

  delay(500);
}