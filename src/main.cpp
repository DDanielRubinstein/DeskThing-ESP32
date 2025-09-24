#include <Arduino.h>

#include "../lib/WiFiHelper/WiFiHelper.h"
#include "../lib/AuthServer/AuthServer.h"
#include "../lib/SpotifyHelper/SpotifyHelper.h"
#include "../lib/VolumeKnob/VolumeKnob.h"

#define BTN_PIN 25
#define CLKW_PIN 32
#define CCLKW_PIN 33  

int last_volume = 0;
bool volume_changed_buffer = false;
unsigned long last_volume_modified = 0;

void setup() {
  Serial.begin(9600);

  WiFiHelper::connect();
  AuthServer::setupServer();
  
  while (AuthServer::authorization_code.empty()) {
    AuthServer::listener();
  }

  Serial.println("Authorization code retreived.");

  SpotifyHelper::setupHelper(AuthServer::authorization_code);

  VolumeKnob::pinSetup(BTN_PIN, CLKW_PIN, CCLKW_PIN);
}

void loop() {

  VolumeKnob::listener();

  if (VolumeKnob::volume != last_volume) {
    Serial.printf("Volume: %d\n", VolumeKnob::volume);
    last_volume = VolumeKnob::volume;
    last_volume_modified = millis();
    
    volume_changed_buffer = true;
  }

  if (volume_changed_buffer && millis() - last_volume_modified >= 500) {
    SpotifyHelper::setVolume(VolumeKnob::volume);

    volume_changed_buffer = false;
  }
}