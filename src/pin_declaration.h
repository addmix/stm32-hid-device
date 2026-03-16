#pragma once
#include <map>
#include <Arduino.h>
#define ADC_MAX_VALUE 1024

struct Pin {
    PinName pin_name = NC;
    int value = 0;
    bool analog = false;
    int inactive_value = HIGH;
    int center = ADC_MAX_VALUE / 2; 
    int range = ADC_MAX_VALUE / 2;
    
    Pin() = default;
    Pin(PinName pin_name, bool analog = false, int inactive_value = HIGH) : pin_name(pin_name), analog(analog), inactive_value(inactive_value) {};

    void update() {
        if (analog) value = analogRead(pin_name) - center;
        else value = digitalRead(pin_name);
    }
};

extern std::map<uint, Pin> pin_map;