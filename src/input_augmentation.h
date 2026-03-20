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

            
            //apply rotation to both pins simultaneously
            //TODO change this, I don't really like how centering is done at this step
            Vector2 rotated_input = Vector2((float) pin_map[pin].value, (float) pin_map[secondary_pin].value).rotate(control_rotation);
            //Serial.println((String) pin_mapping->value + ", " + (String) secondary_pin_mapping->value + "\t\t" + (String) rotated_input.x + ", " + (String) rotated_input.y);

            pin_map[pin].value = rotated_input.x;
            //secondary_pin->value = rotated_input.y;

            break;
        }
    }
};