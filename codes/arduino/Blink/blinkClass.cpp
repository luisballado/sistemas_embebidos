#include "blinkClass.h"

blinkClass::blinkClass(unsigned int pN, unsigned long period){

  pinNumber = pN;
  delayPeriod = period;

}

void blinkClass::initFunction(){
  pinMode(pinNumber,OUTPUT);

}

void blinkClass::blinkLED(){
  digitalWrite(pinNumber,HIGH);
  delay(delayPeriod);
  digitalWrite(pinNumber,LOW );
  delay(delayPeriod);
}