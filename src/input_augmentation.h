#pragma once
#include "utils.h"
#include "pin_mapping.h"

enum AugmentType{
    NONE,
    ROTATION,
};

struct InputAugmentation {
    PinName pin = NC;
    PinName secondary_pin = NC;

    AugmentType type = NONE;
    float control_rotation = 0.0;

    InputAugmentation() = default;

    static InputAugmentation rotation(PinName primary_pin, PinName secondary_pin, float rotation) {
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

    
    #define AUGMENT_BYTE_SIZE 7U //pin number, analog
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

    //TODO
    //static func from_bytes(bytes : PackedByteArray) -> Augmentation:
	//if bytes.size() != BYTE_SIZE:
	//	push_error("Passed byte array does not match the byte size of Augmentation.")
	//
	//var _pin : int = bytes[0]
	//var _secondary_pin : bool = bytes[1]
	//var _type : int = bytes[2]
	//
	//var rotation_bytes := bytes.slice(3, 7)
	//rotation_bytes.reverse()
	//var _control_rotation : float = rotation_bytes.decode_float(0)
	//
	//return Augmentation.new(_pin, _secondary_pin, _type, _control_rotation)
    //
    //static func from_byte_array(bytes : PackedByteArray) -> Array[Augmentation]:
	//var array : Array[Augmentation] = []
	//
	//for index in range(0, bytes.size(), BYTE_SIZE):
	//	array.append(from_bytes(bytes.slice(index, index + BYTE_SIZE)))
	//
	//return array
};