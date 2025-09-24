#include <WiFiClientSecure.h>

#include "../Env/env.h"

namespace WiFiHelper {
    void connect() {
        // set wifi mode to station
        WiFi.mode(WIFI_STA);
    
        // attemp connection using credentials
        WiFi.begin(env::ssid.c_str(), env::pass.c_str());
        
        Serial.print("Connecting to ");
        Serial.print(env::ssid.c_str());
        Serial.print("\n");
        
        while(WiFi.status() != WL_CONNECTED) {
            Serial.print("Status: (");
            Serial.print(WiFi.status());
            Serial.print(")\n");
            delay(500);
        }

        Serial.print("Successfully connected. Local IP address:");
        Serial.println(WiFi.localIP());
    }
}