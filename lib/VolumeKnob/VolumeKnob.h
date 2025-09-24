namespace VolumeKnob {
    extern int volume;
    extern bool mute;
    
    void pinSetup(const int& btn_pin, const int& clkw_pin, const int& cclkw_pin);
    
    void listener();
}