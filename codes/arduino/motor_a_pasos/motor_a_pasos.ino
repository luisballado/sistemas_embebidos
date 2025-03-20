int en = 33;
int in1 = 23;
int in2 = 32;
int in3 = 25;
int in4 = 26;
const int sw = 13;
int swstate;
const int t = 10;
int rotations = 0; // Variable to store rotation count
const int steps_per_rev = 200; // Adjusted to match the stepper motor

void enableMotor() {
  analogWrite(en, 200);
}

void disableMotor() {
  analogWrite(en, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void cw() {
  enableMotor();
  for (int step = 0; step < steps_per_rev; step += 4) {
    digitalWrite(in1, LOW);  digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);  digitalWrite(in4, HIGH);  delay(t);
    digitalWrite(in1, LOW);  digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH); digitalWrite(in4, LOW);   delay(t);
    digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH); digitalWrite(in4, LOW);   delay(t);
    digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);  digitalWrite(in4, HIGH);  delay(t);
  }
  disableMotor();
}

void ccw() {
  enableMotor();
  for (int step = 0; step < steps_per_rev; step += 4) {
    digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);  digitalWrite(in4, HIGH);  delay(t);
    digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH); digitalWrite(in4, LOW);   delay(t);
    digitalWrite(in1, LOW);  digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH); digitalWrite(in4, LOW);   delay(t);
    digitalWrite(in1, LOW);  digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);  digitalWrite(in4, HIGH);  delay(t);
  }
  disableMotor();
}

void setup() {
  pinMode(en, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(sw, INPUT_PULLUP);
  Serial.begin(115200);  // Initialize serial communication
  disableMotor();
}

void loop() {
  if (Serial.available() > 0) {
    int targetRotations = Serial.parseInt();
    if (targetRotations >= 0 && targetRotations <= 9) {
      while (rotations < targetRotations) {
        swstate = digitalRead(sw);
        if (swstate == HIGH) {
          cw();
        } else {
          ccw();
        }
        rotations++;
        Serial.print("Rotation: ");
        Serial.println(rotations);
      }
      rotations = 0;
    } else {
      Serial.println("Enter a value between 0 and 9");
    }
  }
}
