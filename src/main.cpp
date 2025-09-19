#include <Arduino.h>

#include "../lib/VolumeKnob/VolumeKnob.h"

#define BTN_PIN 25
#define CLKW_PIN 32
#define CCLKW_PIN 33

int lastVolume = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  KnobPinSetup(BTN_PIN, CLKW_PIN, CCLKW_PIN);
}

void loop() {
  // put your main code here, to run repeatedly:
  KnobListener();

  if (volume != lastVolume) {
    Serial.println(volume);
    lastVolume = volume;
  }
}