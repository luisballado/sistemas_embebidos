//PUSH BUTTON
int a=22;
int b=21;
int c=2;
int d=4;
int e=5;
int f=19;
int g=18;

struct Button {
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
};

Button button1 = {13, 0, false};

//variables to keep track of the timing of recent interrupts
unsigned long button_time = 0;  
unsigned long last_button_time = 0; 

void IRAM_ATTR isr() {
  button_time = millis();
  if (button_time - last_button_time > 250){
    button1.numberKeyPresses++;
    button1.pressed = true;
    last_button_time = button_time;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(button1.PIN, INPUT_PULLUP);
  pinMode(15, OUTPUT);
  attachInterrupt(button1.PIN, isr, FALLING);
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
}

void loop() {
  if (button1.pressed) {
      digitalWrite(15, HIGH);
      Serial.printf("Button has been pressed %u times\n", button1.numberKeyPresses);
      delay(100);
      digitalWrite(15, LOW);
      button1.pressed = false;
  }
  // for 0
 digitalWrite(a,1);
 digitalWrite(b,0);
 digitalWrite(c,0);
 digitalWrite(d,0);
 digitalWrite(e,0);
 digitalWrite(f,0);
 digitalWrite(g,0);
 delay(80);
 digitalWrite(a,0);
 digitalWrite(b,1);
 digitalWrite(c,0);
 digitalWrite(d,0);
 digitalWrite(e,0);
 digitalWrite(f,0);
 digitalWrite(g,0);
 delay(80);
 digitalWrite(a,0);
 digitalWrite(b,0);
 digitalWrite(c,1);
 digitalWrite(d,0);
 digitalWrite(e,0);
 digitalWrite(f,0);
 digitalWrite(g,0);
 delay(80); 
 digitalWrite(a,0);
 digitalWrite(b,0);
 digitalWrite(c,0);
 digitalWrite(d,1);
 digitalWrite(e,0);
 digitalWrite(f,0);
 digitalWrite(g,0);
 delay(80); 
 digitalWrite(a,0);
 digitalWrite(b,0);
 digitalWrite(c,0);
 digitalWrite(d,0);
 digitalWrite(e,1);
 digitalWrite(f,0);
 digitalWrite(g,0);
 delay(80); 
 digitalWrite(a,0);
 digitalWrite(b,0);
 digitalWrite(c,0);
 digitalWrite(d,0);
 digitalWrite(e,0);
 digitalWrite(f,1);
 digitalWrite(g,0);
 delay(80);
}
