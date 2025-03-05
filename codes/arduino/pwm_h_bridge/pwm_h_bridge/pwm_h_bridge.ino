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
int dutyCycle = 200;
volatile unsigned long pulseCount = 0;
void IRAM_ATTR handlePulse(){
  pulseCount++;
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
  pinMode(enable1Pin, OUTPUT);
  
  // configure LEDC PWM
  ledcAttachChannel(enable1Pin, freq, resolution, pwmChannel);

  Serial.begin(115200);

  //pinMode(34,INPUT);
  //attachInterrupt(digitalPinToInterrupt(34),handlePulse,RISING);
  // testing
  Serial.print("Testing DC Motor...");
}

void loop() {

  // Move DC motor forward with increasing speed
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  while (dutyCycle <= 255){
    ledcWrite(enable1Pin, dutyCycle);   
    Serial.print("Forward with duty cycle: ");
    Serial.println(dutyCycle);
    dutyCycle = dutyCycle + 5;
    //Serial.print("Pulsos: ");
    //Serial.print(pulseCount);
    if(dutyCycle == 200){
      digitalWrite(a,1);
      digitalWrite(b,0);
      digitalWrite(c,0);
      digitalWrite(d,0);
      digitalWrite(e,0);
      digitalWrite(f,0);
      digitalWrite(g,0);
    }
    if(dutyCycle == 210){
      digitalWrite(a,0);
      digitalWrite(b,1);
      digitalWrite(c,1);
      digitalWrite(d,0);
      digitalWrite(e,0);
      digitalWrite(f,0);
      digitalWrite(g,0);
    }

    if(dutyCycle == 220){
      digitalWrite(a,1);
      digitalWrite(b,1);
      digitalWrite(c,0);
      digitalWrite(d,1);
      digitalWrite(e,1);
      digitalWrite(f,0);
      digitalWrite(g,1);
    }

    if(dutyCycle == 230){
      digitalWrite(a,1);
      digitalWrite(b,1);
      digitalWrite(c,1);
      digitalWrite(d,1);
      digitalWrite(e,0);
      digitalWrite(f,0);
      digitalWrite(g,1);
    }

    if(dutyCycle == 240){
      digitalWrite(a,0);
      digitalWrite(b,1);
      digitalWrite(c,1);
      digitalWrite(d,0);
      digitalWrite(e,0);
      digitalWrite(f,1);
      digitalWrite(g,1);
    }

    if(dutyCycle == 250){
      digitalWrite(a,1);
      digitalWrite(b,0);
      digitalWrite(c,1);
      digitalWrite(d,1);
      digitalWrite(e,0);
      digitalWrite(f,1);
      digitalWrite(g,1);
    }

    delay(1000);
  }

   // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  //Serial.print("Pulsos: ");
  //Serial.print(pulseCount);
  delay(500);

  // Move DC motor backwards at maximum speed
  Serial.println("Moving Backwards");
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  //Serial.print("Pulsos: ");
  //Serial.print(pulseCount);
  delay(2000);

    // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  //Serial.print("Pulsos: ");
  //Serial.print(pulseCount);
  delay(500);

    // Move the DC motor forward at maximum speed
  Serial.println("Moving Forward");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  //dutyCycle = 200; 
  //Serial.print("Pulsos: ");
  //Serial.print(pulseCount);
  delay(2000);

  // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  //Serial.print("Pulsos: ");
  //Serial.print(pulseCount);
  delay(500);
    
  dutyCycle = 200;
}
