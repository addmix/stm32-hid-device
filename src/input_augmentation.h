#pragma once
#include "utils.h"
#include "pin_mapping.h"

enum AugmentType{
    NONE,
    ROTATION,
};

struct InputAugmentation {
    Pin* pin = nullptr;
    Pin* secondary_pin = nullptr;

    AugmentType type = NONE;
    float control_rotation = 0.0;

    InputAugmentation() = default;

    static InputAugmentation rotation(Pin* primary_pin, Pin* secondary_pin, float rotation) {
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
            //check that both pin mappings are valid, and are not equal
            if (pin == nullptr or secondary_pin == nullptr) break;

            
            //apply rotation to both pins simultaneously
            //TODO change this, I don't really like how centering is done at this step
            Vector2 rotated_input = Vector2((float) pin->value, (float) secondary_pin->value).rotate(control_rotation);
            //Serial.println((String) pin_mapping->value + ", " + (String) secondary_pin_mapping->value + "\t\t" + (String) rotated_input.x + ", " + (String) rotated_input.y);

            pin->value = rotated_input.x;
            //secondary_pin->value = rotated_input.y;

            break;
        }
    }
};