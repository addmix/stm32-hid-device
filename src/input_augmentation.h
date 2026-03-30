#pragma once
#include "utils.h"
#include "pin_mapping.h"

enum AugmentType{
    NONE,
    ROTATION,
};

#define AUGMENT_BYTE_SIZE 7U //pin, secondary pin, type, 4 byte rotation
struct InputAugmentation {
    static const u_int8_t BYTE_SIZE = AUGMENT_BYTE_SIZE;
    PinName pin = NC;
    PinName secondary_pin = NC;

    AugmentType type = NONE;
    float control_rotation = 0.0;

    InputAugmentation() = default;
    InputAugmentation(PinName primary_pin, PinName secondary_pin, AugmentType type = ROTATION, float rotation = 0.0) {
        this->pin = primary_pin;
        this->secondary_pin = secondary_pin;
        this->type = type;
        this->control_rotation = rotation;
    }

    static InputAugmentation rotation(PinName primary_pin, PinName secondary_pin, AugmentType type, float rotation) {
        InputAugmentation aug = InputAugmentation();
        
        aug.pin = primary_pin;
        aug.secondary_pin = secondary_pin;

        aug.type = ROTATION;
        aug.control_rotation = rotation;

        return aug;
    }

    //only apply augments when input was updated this iteration
    void apply_augmentations() {
        switch (type) {
        case ROTATION:
            //check that both pin mappings are valid
            if (pin == NC or secondary_pin == NC) break;

            auto pin_obj = pin_map.find(pin)->second;
            auto secondary_pin_obj = pin_map.find(secondary_pin)->second;
            
            //apply rotation to both pins simultaneously
            Vector2 rotated_input = Vector2((float) pin_obj.value, (float) secondary_pin_obj.value).rotate(control_rotation);

            pin_obj.value = rotated_input.x;
            //we intentionally do not apply rotation to the secondary axis to allow separate rotation amounts
            //secondary_pin->value = rotated_input.y;

            break;
        }
    }

    void print() {
        Serial.println(
        "pin number=" + (String) pin +
        " secondary pin number=" + (String) secondary_pin +
        " type=" + (String) type +
        " rotation=" + (String) control_rotation
        );
    }
    
    void to_bytes(u_int8_t *&return_buffer) const {
        *return_buffer++ = pin;
        *return_buffer++ = secondary_pin;
        *return_buffer++ = type;

        u_int32_t rotation_bits = 0;
        memcpy(&rotation_bits, &control_rotation, 4);
        *return_buffer++ = rotation_bits >> 24 & 0xFF;
        *return_buffer++ = rotation_bits >> 16 & 0xFF;
        *return_buffer++ = rotation_bits >> 8 & 0xFF;
        *return_buffer++ = rotation_bits & 0xFF;
    }


    static InputAugmentation from_bytes(u_int8_t *&read_buffer) {//static func from_bytes(bytes : PackedByteArray) -> Augmentation:
        //if bytes.size() != BYTE_SIZE:
        //	push_error("Passed byte array does not match the byte size of Augmentation.")
        
        PinName pin = (PinName) *read_buffer++;//var _pin : int = bytes[0]
        PinName secondary_pin = (PinName) *read_buffer++;//var _secondary_pin : bool = bytes[1]
        AugmentType type = (AugmentType) *read_buffer++;//var _type : int = bytes[2]
        
        u_int32_t rotation_bytes = ((u_int16_t) *read_buffer++ << 24 ) + ((u_int16_t) *read_buffer++ << 16 ) + ((u_int16_t) *read_buffer++ << 8 ) + (u_int16_t) *read_buffer++;
        float rotation = 0x00000000;
        memcpy(&rotation, &rotation_bytes, 4);
        //var rotation_bytes := bytes.slice(3, 7)
        //rotation_bytes.reverse()
        //var _control_rotation : float = rotation_bytes.decode_float(0)
        
        return InputAugmentation(pin, secondary_pin, type, rotation);
    }

    static void from_byte_array(u_int8_t *&read_buffer, InputAugmentation *&return_buffer, u_int16_t items_to_parse) {//static func from_byte_array(bytes : PackedByteArray) -> Array[Augmentation]:
        for (size_t i = 0; i < items_to_parse; ++i) {
            *return_buffer++ = from_bytes(read_buffer);
        }
    }
};