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
};