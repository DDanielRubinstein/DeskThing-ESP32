namespace VolumeKnob {
    extern int volume;
    extern bool mute;
    
    void KnobPinSetup(int btn_pin, int clkw_pin, int cclkw_pin);
    
    void KnobListener();
}