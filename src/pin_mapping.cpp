#include "pin_mapping.h"

void PinMapping::update_value() {
    if (pin_name != NC) this->pin = &pin_map[pin_name];
    previous_value = value;

    if (pin->is_bounce() and not pin->analog) return; //debounce implementation

    int new_value = read_value();

    //apply deadzone
    if (pin->analog and abs(new_value) < (int) abs((float)max_report_value * deadzone_percent)) {
        new_value = 0;
    }
    //TODO problem with this logic: because input augments modify the value of the pin mapping, this "change amount before update" check almost always fails.
    //prevents over-reporting of thumbstick movements as there is noise in the ADC
    if (pin->analog and abs(previous_value - new_value) < change_amount_before_update) {
        //return;
    }
    value = new_value;
}

const int PinMapping::get_value_digital() {
    bool is_activated = pin_map[pin_name].value != pin_map[pin_name].inactive_value;

    int temp_value = (int) is_activated;

    if (is_activated) temp_value = 1;
    else temp_value = 0;

    if (invert) {
        temp_value = -temp_value;
    }

    //apply scaling
    //TODO fix issue that causes this to always return 0 due to float > int casting or something
    //value = (int) ((float) value * scale);

    return temp_value;
}

const int PinMapping::get_value_analog() {
    int temp_value = pin_map[pin_name].value;

    //center the ADC value to 0
    //commented out because the pin declaration will handle centering
    //temp_value = temp_value - center;

    //apply scaling
    temp_value = (int) (temp_value * scale);

    //clamping
    temp_value = min(max(temp_value, -max_report_value / 2 + 1), max_report_value / 2 - 1);

    if (invert) {
        temp_value = -temp_value;
    }
    return temp_value;
}
const int PinMapping::read_value() {
    if (pin->analog) {
        return get_value_analog();
    }

    return get_value_digital();
}

const int PinMapping::get_value() {
    return value;
}

const bool PinMapping::is_pressed() {
    return is_pressed(value);
}
const bool PinMapping::is_pressed(int test_value) {
    if (pin->analog) {
        //TODO implement quick-release here
        return test_value >= activation_value;
    }

    return test_value != 0;//TODO: change this to have scaling compatibility
}
const bool PinMapping::is_released() {
    return is_released(value);
}
const bool PinMapping::is_released(int test_value) {
    if (pin->analog) {
        return not is_pressed(test_value);
    }

    return test_value == 0;//TODO: change this to have scaling compatibility
}
const bool PinMapping::is_just_pressed() {
    return is_pressed() and is_released(previous_value); //TODO maybe add a third condition that the change in value has to be significant, to avoid ADC noise?
}
//TODO possible issues with is_just_released()
const bool PinMapping::is_just_released() {
    return is_released() and is_pressed(previous_value);
}
const bool PinMapping::is_just_changed() {
    return is_just_pressed() or is_just_released();
}
