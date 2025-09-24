#include <iostream>
#include <string.h>

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "mbedtls/base64.h"

#include "../Env/env.h"
#include "../AuthServer/AuthServer.h"

using std::string;

namespace SpotifyHelper {
    const char* spotify_root_ca_cert = \
    "    -----BEGIN CERTIFICATE-----\n" \
    "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n" \
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n" \
    "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n" \
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n" \
    "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n" \
    "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n" \
    "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n" \
    "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n" \
    "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n" \
    "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n" \
    "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n" \
    "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n" \
    "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n" \
    "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n" \
    "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n" \
    "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n" \
    "MrY=\n" \
    "-----END CERTIFICATE-----\n";

    const string spotify_token_api_uri = "https://accounts.spotify.com/api";
    const string spotify_api_uri = "https://api.spotify.com/v1/me";
    const string redirect_uri = "https://esp.surf-pads.com/callback";
    
    string access_token, refresh_token;
    unsigned long token_requested_at;
    
    void setupHelper(const string& authorization_token) {
        if (authorization_token.empty()) {
            Serial.println("Authorization code has not been set.");
            return;
        } else if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi connection has not been set.");
            return;
        }

        WiFiClientSecure wifi_client;
        HTTPClient http_client;

        // set spotify root ca cert to allow https requests
        wifi_client.setCACert(spotify_root_ca_cert);
        
        // concat client id with secret
        std::string auth_buffer = env::client_id + ":" + env::client_secret;
        // prepare buffer for base64 encoding
        unsigned char encoded_buffer[4 * ((auth_buffer.length() + 2) / 3) + 1];
        size_t outlen;

        // base64 encoding
        int ret = mbedtls_base64_encode(
            encoded_buffer,
            sizeof(encoded_buffer),
            &outlen,
            reinterpret_cast<const unsigned char*>(auth_buffer.c_str()),
            auth_buffer.length()
        );

        // if encoding successful
        if (ret == 0) {
            encoded_buffer[outlen] = '\0';
            // get access token
            if (http_client.begin(wifi_client, (spotify_token_api_uri + "/token").c_str())) {
                http_client.addHeader("Content-Type", "application/x-www-form-urlencoded");
                http_client.addHeader("Authorization", ("Basic " + string((char*) encoded_buffer)).c_str());
    
                string form_data = "code=" + authorization_token + "&" + "redirect_uri=" + redirect_uri + "&" + "grant_type=authorization_code";
                
                http_client.setReuse(false);
                int resp_code = http_client.POST(form_data.c_str());

                if (resp_code == 200) {
                    JsonDocument resp;

                    DeserializationError error = deserializeJson(resp, http_client.getString());

                    access_token = resp["access_token"].as<string>();
                    refresh_token = resp["refresh_token"].as<string>();
                    token_requested_at = millis();

                    Serial.println("Access code retreived.");
                    Serial.println(access_token.c_str());
                } else {
                    Serial.printf("Error. Access token request returned %d\n", resp_code);
                }

            } else {
                Serial.println("Error. Failed to connect to Spotify API.");
            }
            
        }

        http_client.end();
    }
    
    void setVolume(const unsigned int& volume) {
        WiFiClientSecure wifi_client;
        HTTPClient http_client;

        // set spotify root ca cert to allow https requests
        wifi_client.setCACert(spotify_root_ca_cert);

        if (http_client.begin(wifi_client, (spotify_api_uri + "/player/volume" + "?volume_percent=" + std::to_string(volume)).c_str())) {
            http_client.addHeader("Authorization", ("Bearer " + access_token).c_str());
            http_client.addHeader("Content-Length", "0");

            http_client.setReuse(false);
            int resp_code = http_client.PUT("");

            if (resp_code == 204) {
                Serial.printf("Remote volume set to %d\n", volume);
            } else {
                Serial.printf("Something went wrong. (%d)", resp_code);
                Serial.println(http_client.getString());
            }
        } else {
            Serial.println("Error. Failed to connect to Spotify API.");
        }

        http_client.end();
    }
}