#pragma once
#include <map>
#include <Arduino.h>
#define ADC_MAX_VALUE 1024
#define DEBOUNCE_TICKS_MS 15

struct Pin {
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

    #define PIN_BYTE_SIZE 4U //pin number, analog, inactive value, inactive value
    void to_bytes(u_int8_t *&return_buffer) const {
        *return_buffer++ = pin_name;
        *return_buffer++ = analog;
        *return_buffer++ = inactive_value >> 8 & 0xFF;
        *return_buffer++ = inactive_value & 0xFF;
    }

    //TODO
    //static func from_bytes(bytes : PackedByteArray) -> PinDeclaration:
	//if bytes.size() != BYTE_SIZE:
	//	push_error("Passed byte array does not match the byte size of PinDeclaration.")
	//
	//var _pin_name : int = bytes[0]
	//var _analog : bool = bytes[1]
	//var _inactive_value : int = (bytes[2] << 8) + bytes[3]
	//
	//return PinDeclaration.new(_pin_name, _analog, _inactive_value)
    //
    //static func from_byte_array(bytes : PackedByteArray) -> Array[PinDeclaration]:
	//var array : Array[PinDeclaration] = []
	//
	//for index in range(0, bytes.size(), BYTE_SIZE):
	//	array.append(from_bytes(bytes.slice(index, index + BYTE_SIZE)))
	//
	//return array
};

extern std::map<uint, Pin> pin_map;