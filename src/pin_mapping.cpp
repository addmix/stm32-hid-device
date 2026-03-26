#include "pin_mapping.h"

void PinMapping::update_value() {
    if (pin_name == NC) return;
    //TODO: make these pin_map.find(pin_name) calls less repetetive
    auto it = pin_map.find(pin_name);
    if (it == pin_map.end()) {
        value = 0;
        return;
    }
    
    Pin& pin = it->second;
    
    previous_value = value;

    if (pin.is_bounce() and not pin.analog) return; //debounce implementation
    //TODO: it appears that positive axis values have discrete "steps", where negative axis values are smooth.
    int new_value = read_value();

    //apply deadzone
    //TODO: fix deadzone implementation so that there isn't a "jump" when you exit the deadzone, where the edge of the deadzone is 0, instead of the center
    //part of this could also be a circular deadzone implementation
    if (pin.analog and (abs(new_value) < (deadzone / 255.0) * max_report_value) ) {
        value = 0;
        return;
    }
    
    //prevents over-reporting of thumbstick movements as there is noise in the ADC
    if (pin.analog and abs(previous_value - new_value) < change_amount_before_update) {
        return;
    }
    
    value = new_value;
}

int PinMapping::get_value_digital() const {
    //TODO: make these pin_map.find(pin_name) calls less repetetive
    auto it = pin_map.find(pin_name);
    if (it == pin_map.end()) {
        return 0;
    }
    Pin& pin = it->second;

    bool is_activated = pin.value != pin.inactive_value;

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

int PinMapping::get_value_analog() const {
    //TODO: make these pin_map.find(pin_name) calls less repetetive
    auto it = pin_map.find(pin_name);
    if (it == pin_map.end()) {
        return 0;
    }
    Pin& pin = it->second;

    int temp_value = pin.value;
    
    //apply scaling
    temp_value = (int) (temp_value * scale);

    //clamping
    temp_value = min(max(temp_value, -max_report_value / 2 + 1), max_report_value / 2 - 1);

    if (invert) {
        temp_value = -temp_value;
    }
    return temp_value;
}
int PinMapping::read_value() const {
    //TODO: make these pin_map.find(pin_name) calls less repetetive
    auto it = pin_map.find(pin_name);
    if (it == pin_map.end()) {
        return 0;
    }
    Pin& pin = it->second;

    if (pin.analog) {
        return get_value_analog();
    }

    return get_value_digital();
}

int PinMapping::get_value() const {
    return value;
}

bool PinMapping::is_pressed() const {
    return is_pressed(value);
}
bool PinMapping::is_pressed(int test_value) const {
    //TODO: make these pin_map.find(pin_name) calls less repetetive
    auto it = pin_map.find(pin_name);
    if (it == pin_map.end()) {
        return false;
    }
    Pin& pin = it->second;

    if (pin.analog) {
        //TODO implement quick-release here
        return test_value >= activation_value;
    }

    return test_value != 0;
}
bool PinMapping::is_released() const {
    return is_released(value);
}
bool PinMapping::is_released(int test_value) const {
    //TODO: make these pin_map.find(pin_name) calls less repetetive
    auto it = pin_map.find(pin_name);
    if (it == pin_map.end()) {
        return false;
    }
    Pin& pin = it->second;

    if (pin.analog) {
        return not is_pressed(test_value);
    }

    return test_value == 0;
}
bool PinMapping::is_just_pressed() const {
    return is_pressed() and is_released(previous_value);
}
bool PinMapping::is_just_released() const {
    return is_released() and is_pressed(previous_value);
}
bool PinMapping::is_just_changed() const {
    return is_just_pressed() or is_just_released();
}


