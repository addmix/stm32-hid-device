#pragma once
#include "pin_mapping.h"
#include "input_enums.h"

extern hid_mouse_report_t mouse;
extern hid_gamepad_report_t gamepad;

const uint8_t handleMouseButtons(PinMapping &pin_mapping) ;
const int8_t handleMouseMovementX(PinMapping &pin_mapping);
const int8_t handleMouseMovementY(PinMapping &pin_mapping);
const int8_t handleMouseScroll(PinMapping &pin_mapping);
const int8_t handleMousePan(PinMapping &pin_mapping);
const uint32_t handleGamepadButtons(PinMapping &pin_mapping);
const uint8_t handleGamepadHat(PinMapping &pin_mapping);
void handleGamepadAxis(PinMapping &pin_mapping);