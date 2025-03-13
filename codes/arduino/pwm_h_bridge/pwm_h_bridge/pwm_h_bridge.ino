// DISPLAY
int a=22;
int b=21;
int c=2;
int d=4;
int e=5;
int f=19;
int g=18;

// Motor A
int motor1Pin1 = 23; 
int motor1Pin2 = 32; 
int enable1Pin = 33; 

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
volatile unsigned long pulseCount = 0;

// ----- Encoder configuration -----
const int encoderPinA = 34;      // Encoder signal A pin
const int encoderPinB = 35;      // Encoder signal B pin
volatile long encoderCount = 0;  // Encoder pulse counter

void IRAM_ATTR encoderISR_A() {
  int b = digitalRead(encoderPinB);
  if (b == HIGH) {
    encoderCount++;
  } else {
    encoderCount--;
  }
}

void IRAM_ATTR encoderISR_B() {
  int a = digitalRead(encoderPinA);
  if (a == HIGH) {
    encoderCount--;
  } else {
    encoderCount++;
  }
}

// Timing variables for velocity update (every 1000ms) and display multiplexing
unsigned long lastVelocityUpdate = 0;
const unsigned long velocityInterval = 1000; // update every 1 second
unsigned long lastDisplayRefresh = 0;
const unsigned long displayRefreshInterval = 5; // refresh each digit every 5ms
int currentDigit = 0;  // 0 for tens, 1 for ones

// PID Parameters
float Kp = 1.2;  // Proportional gain
float Ki = 0.02; // Integral gain
float Kd = 0.5;  // Derivative gain

void rotateMotor(float rotations) {
  const long PULSES_PER_ROTATION = 360;  // NXT motor encoder resolution
  long targetPulses = abs(rotations) * PULSES_PER_ROTATION;

  long prevError = 0;
  float integral = 0;
  
  // Reset encoder count
  noInterrupts();
  encoderCount = 0;
  interrupts();

  // Set motor direction
  if (rotations > 0) {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
  } else {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
  }

  unsigned long lastTime = millis();

  while (true) {
    // Compute PID control
    long currentPulses = abs(encoderCount);
    long error = targetPulses - currentPulses;
    
    if (error <= 0) break;  // Stop if we reach the target

    // Time difference
    unsigned long now = millis();
    float deltaTime = (now - lastTime) / 1000.0;
    lastTime = now;

    // PID calculations
    integral += error * deltaTime;
    float derivative = (error - prevError) / deltaTime;
    prevError = error;

    float output = (Kp * error) + (Ki * integral) + (Kd * derivative);
    int pwmValue = constrain(output, 0, 255); // Keep PWM in a valid range

    analogWrite(enable1Pin, pwmValue);
    
    Serial.print("Target: "); Serial.print(targetPulses);
    Serial.print(" | Current: "); Serial.print(currentPulses);
    Serial.print(" | PWM: "); Serial.println(pwmValue);

    delay(10); // Small delay for stability
  }

  // Stop motor
  analogWrite(enable1Pin, 0);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  Serial.println("Rotation complete");
}




void setup() {
  // DISPLAY
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);

  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);

  
  
  // configure LEDC PWM
  ledcAttachChannel(enable1Pin, freq, resolution, pwmChannel);

  Serial.begin(115200);

  //pinMode(34,INPUT);
  //attachInterrupt(digitalPinToInterrupt(34),handlePulse,RISING);
  // testing
  Serial.print("Testing DC Motor...");

  // ----- Setup Encoder Inputs and Interrupts -----
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), encoderISR_A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), encoderISR_B, CHANGE);

  lastVelocityUpdate = millis();
  lastDisplayRefresh = millis();
}

void loop() {

  rotateMotor(-5);
  
  Serial.print("\nEncoder count: ");
  Serial.print(encoderCount);
  delay(1000);

  rotateMotor(5);
  
}
