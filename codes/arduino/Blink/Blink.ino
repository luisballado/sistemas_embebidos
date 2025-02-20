#include"blinkClass.h"

unsigned int ledPin = 4;
unsigned long delayN = 50;

unsigned int ledPin2 = 2;

blinkClass Blink1(ledPin,delayN);
blinkClass Blink2(ledPin2,delayN);

void setup() {
  Blink1.initFunction();
  Blink2.initFunction();
}

void loop() {
  Blink1.blinkLED();
  Blink2.blinkLED();
}