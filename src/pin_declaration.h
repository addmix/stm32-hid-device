#pragma once
#include <map>
#include <Arduino.h>
#define ADC_MAX_VALUE 1024
#define DEBOUNCE_TICKS_MS 15

#define PIN_BYTE_SIZE 4U //pin number, analog, inactive value, inactive value
struct Pin {
    static const u_int8_t BYTE_SIZE = PIN_BYTE_SIZE;
    PinName pin_name = NC;
    bool analog = false;
    u_int16_t inactive_value = HIGH;
    int value = 0;
    u_int last_change_time = -DEBOUNCE_TICKS_MS;
    int center = ADC_MAX_VALUE / 2; 
    //int range = ADC_MAX_VALUE / 2;
    
    Pin() = default;
    Pin(PinName pin_name, bool analog = false, int inactive_value = HIGH) : pin_name(pin_name), analog(analog), inactive_value(inactive_value) {};

    void update() {
        if (is_bounce() and not analog) return;
        int new_value = 0;
        if (analog) new_value = analogRead(pin_name) - center;
        else new_value = digitalRead(pin_name);

        if (value != new_value) {
            last_change_time = millis();

            //if (not analog) Serial.println("value changed " + (String) value + (String) new_value);
        }

        value = new_value;
    }
    const bool is_bounce() {
        return (millis() - last_change_time) <= DEBOUNCE_TICKS_MS;
    }

    void print() {
        Serial.println(
            "pin number=" + (String) pin_name + 
            " analog=" + (String) analog +
            " inactive value=" + (String) inactive_value
        );
    }
    
    void to_bytes(u_int8_t *&return_buffer) const {
        *return_buffer++ = pin_name;
        *return_buffer++ = analog;
        *return_buffer++ = inactive_value >> 8 & 0xFF;
        *return_buffer++ = inactive_value & 0xFF;
    }

    static Pin from_bytes(u_int8_t *&read_buffer) {//static func from_bytes(bytes : PackedByteArray) -> PinDeclaration:
        //if bytes.size() != BYTE_SIZE:
        //	push_error("Passed byte array does not match the byte size of PinDeclaration.")
        
        PinName pin_name = (PinName) *read_buffer++;//var _pin_name : int = bytes[0]
        bool analog = *read_buffer++;//var _analog : bool = bytes[1]
        u_int16_t inactive_value = ((u_int16_t) *read_buffer++ << 8 ) + (u_int16_t) *read_buffer++;//var _inactive_value : int = (bytes[2] << 8) + bytes[3]
        
        return Pin(pin_name, analog, inactive_value);//return PinDeclaration.new(_pin_name, _analog, _inactive_value)
    }

    static void from_byte_array(u_int8_t *&read_buffer, Pin *&return_buffer, u_int16_t items_to_parse) {//static func from_byte_array(bytes : PackedByteArray) -> Array[PinDeclaration]:
        for (size_t i = 0; i < items_to_parse; ++i) {
            *return_buffer++ = from_bytes(read_buffer);
        }
    }
};

extern std::map<uint, Pin> pin_map;