#include "input_handlers.h"

hid_mouse_report_t mouse;
hid_gamepad_report_t gamepad;

const uint8_t handleMouseButtons(PinMapping &pin_mapping) {
  //if the input_id is a normal mouse button, and the button is pressed, return it's corresponding bit
  return (pin_mapping.is_pressed() and pin_mapping.input_id <= MOUSE_BUTTON_FORWARD) ? pin_mapping.input_id : 0;
}

const int8_t handleMouseMovementX(PinMapping &pin_mapping) {
  if (not pin_mapping.is_pressed()) return 0;

  switch (pin_mapping.input_id) {
  case MOUSE_MOVE_LEFT:
    return -pin_mapping.get_value();
    break;
  case MOUSE_MOVE_RIGHT:
    return pin_mapping.get_value();
    break;
  default:
    return 0;
  }
}

const int8_t handleMouseMovementY(PinMapping &pin_mapping) {
  if (not pin_mapping.is_pressed()) return 0;

  switch (pin_mapping.input_id) {
  case MOUSE_MOVE_UP:
    return -pin_mapping.get_value();
    break;
  case MOUSE_MOVE_DOWN:
    return pin_mapping.get_value();
    break;
  default:
    return 0;
  }
}

const int8_t handleMouseScroll(PinMapping &pin_mapping) {
  if (not pin_mapping.is_just_pressed()) return 0;
  
  switch (pin_mapping.input_id) {
  case MOUSE_SCROLL_UP:
    return -pin_mapping.get_value();
    break;
  case MOUSE_SCROLL_DOWN:
    return -pin_mapping.get_value();
    break;

  default:
    return 0;
  }
}

const int8_t handleMousePan(PinMapping &pin_mapping) {
  if (not pin_mapping.is_just_pressed()) return 0;

  switch (pin_mapping.input_id) {
  case MOUSE_SCROLL_LEFT:
    return -pin_mapping.get_value();
    break;
  case MOUSE_SCROLL_RIGHT:
    return pin_mapping.get_value();
    break;
  default:
    return 0;
  }
}

const uint32_t handleGamepadButtons(PinMapping &pin_mapping) {
  return (pin_mapping.is_pressed()) ? (uint32_t) pin_mapping.input_id : 0;
}

const uint8_t handleGamepadHat(PinMapping &pin_mapping) {
  return (pin_mapping.is_pressed()) ? pin_mapping.input_id : GAMEPAD_HAT_CENTERED;
}

void handleGamepadAxis(PinMapping &pin_mapping) {
  switch (pin_mapping.input_id) {
  case GAMEPAD_LEFT_ANALOG_X:
    gamepad.x = pin_mapping.get_value();
    break;
  case GAMEPAD_LEFT_ANALOG_Y:
    gamepad.y = pin_mapping.get_value();
    break;
  case GAMEPAD_LEFT_TRIGGER:
    gamepad.rx = pin_mapping.get_value();
    break;
  case GAMEPAD_RIGHT_ANALOG_X:
    gamepad.z = pin_mapping.get_value();
    break;
  case GAMEPAD_RIGHT_ANALOG_Y:
    gamepad.rz = pin_mapping.get_value();
    break;
  case GAMEPAD_RIGHT_TRIGGER:
    gamepad.ry = pin_mapping.get_value();
    break;
  }
}