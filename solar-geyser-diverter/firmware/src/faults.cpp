#include "faults.h"
#include "pins.h"
#include <Arduino.h>

namespace {
  bool faultActive = false;
}

void Faults::begin(){
  pinMode(Pins::LED_FAULT, OUTPUT);
  digitalWrite(Pins::LED_FAULT, LOW);
}

void Faults::raise(const char *reason){
  faultActive = true;
  digitalWrite(Pins::LED_FAULT, HIGH);
  Serial.printf("[FAULT] %s\n", reason);
}

void Faults::clear(){
  faultActive = false;
  digitalWrite(Pins::LED_FAULT, LOW);
  Serial.println("[FAULT] cleared");
}

bool Faults::active(){
  return faultActive;
}
