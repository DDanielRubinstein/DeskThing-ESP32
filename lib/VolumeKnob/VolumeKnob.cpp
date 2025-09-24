#include <Arduino.h>

// pins
int btn_pin = 0, clkw_pin = 0, cclkw_pin = 0;

// clkw and cclkw represent the pins which change states first and second when turned clockwise respectively.
int last_clkw_state;

namespace VolumeKnob {
    int volume = 0;
    bool mute = false;

    void pinSetup(const int& _btn_pin, const int& _clkw_pin, const int& _cclkw_pin) {
        btn_pin = _btn_pin, clkw_pin = _clkw_pin, cclkw_pin = _cclkw_pin;
    
        // set pins to input
        pinMode(btn_pin, INPUT_PULLUP);
        pinMode(clkw_pin, INPUT_PULLUP);
        pinMode(cclkw_pin, INPUT_PULLUP);
    
        last_clkw_state = digitalRead(clkw_pin);
    }
    
    void listener() {
        if (!btn_pin || !clkw_pin || !cclkw_pin) {
            Serial.println("Pin values have not been set.");
            return;
        }
    
        int trigger_mute = !digitalRead(btn_pin);
        int cur_clkw_state = digitalRead(clkw_pin);
    
        /**
         * NOTE: to avoid duplicate registration of the same rotation, check only if clkw pin state changes.
         * also, every pin changes state twice per rotation, so registering a single change is enough. 
         */
        if (cur_clkw_state != last_clkw_state && cur_clkw_state == HIGH) {
            int cur_cclkw_state = digitalRead(cclkw_pin);
            
            // increment counter if knob was turned clkw, decrement otherwise
            volume += (cur_clkw_state != cur_cclkw_state) ? (volume < 100) : -1*(volume > 0);
        }
    
        last_clkw_state = cur_clkw_state;
    
        delay(1);
    }
}