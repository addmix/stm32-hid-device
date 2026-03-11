#include "pin_mapping.h"

PinMapping::PinMapping(
    PinName pin,
    int type,
    u_int16_t id,
    bool isAnalog,
    bool isInverted,
    bool quickRelease,
    int counterStrafeHelpTime
)
    : pin_name(pin), 
    input_type(type), 
    input_id(id), 
    analog(isAnalog), 
    invert(isInverted), 
    //these are unimplemented
    quick_release(quickRelease), 
    counter_strafe_help_time_ms(counterStrafeHelpTime),
    value(0), 
    previous_value(0) 
{
    
}
  
void PinMapping::update_value() {
    previous_value = value;

    if (is_bounce()) return; //debounce implementation

    int new_value = read_value();

    //if the read value is within the deadzone, set the value to centered and return
    if (abs(new_value - center) < (int) ((float)max_report_value * deadzone_percent)) {
        value = center;
    }
    //prevents over-reporting of thumbstick movements as there is noise in the ADC
    else if (analog and abs(previous_value - new_value) < change_amount_before_update) return;

    value = new_value;

    //keep track of the time when the value is changed
    if (is_just_changed()) {
        last_change_time = millis();
    }
}

int PinMapping::get_value_digital() {
    int value = digitalRead(pin_name);

    //TODO adjust this so that the "activated" state can be defined. I want 0 to always be not activated, and 1 to be activated, will similify some of the value == HIGH/LOW checks and multipliers

    if (invert) {
        value = 1 - value;
    }

    //aply scaling
    value = (int) ((float) value * scale);

    return value;
}

int PinMapping::get_value_analog() {
    int value = analogRead(pin_name);

    //center the ADC value to 0
    value = value - center;

    //apply scaling
    value = (int) (value * scale);

    //clamping
    value = min(max(value, -max_report_value / 2), max_report_value / 2);

    if (invert) {
        value = -value;
    }
    return value;
}
int PinMapping::read_value() {
    if (analog) {
        return get_value_analog();
    }

    return get_value_digital();;
}

const int PinMapping::get_value() {
    return value;
}

const bool PinMapping::is_pressed(int test_value) {
    //if no value is supplied, default to the current value
    if (test_value == -1) test_value = value;

    if (analog) {
        //TODO implement quick-release here
        return test_value >= activation_value;
    }

    return test_value == LOW;//TODO: change this to have scaling compatibility
}
const bool PinMapping::is_released(int test_value) {
    //if no value is supplied, default to the current value
    if (test_value == -1) test_value = value;

    if (analog) {
        return not is_pressed(test_value);
    }

    return test_value == HIGH;//TODO: change this to have scaling compatibility
}
const bool PinMapping::is_just_pressed() {
    return is_pressed() and is_released(previous_value);
}
const bool PinMapping::is_just_released() {
    return is_released() and is_pressed(previous_value);
}
const bool PinMapping::is_just_changed() {
    return is_just_pressed() or is_just_released();
}
const bool PinMapping::is_bounce() {
    return (millis() - last_change_time) <= DEBOUNCE_TICKS_MS;
}