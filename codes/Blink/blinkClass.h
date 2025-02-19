#ifndef BLINKCLASS_H
#define BLINKCLASS_H

#include<Arduino.h>

class blinkClass{

  public:
    blinkClass(unsigned int pN, unsigned long period);
    void initFunction();
    void blinkLED();
  private:
    unsigned int pinNumber;
    unsigned long delayPeriod;
};

#endif