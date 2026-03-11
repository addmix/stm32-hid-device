#include "nkro_keyboard.h"

uint8_t nkro_report[NKRO_BYTES];
uint8_t nkro_modifiers = 0;

bool isModifierKey(uint8_t keycode) {
    return keycode >= 0xE0 and keycode <= 0xE7;
}

void pressKeyNKRO(uint8_t keycode) {
  Serial.println(keycode);
  uint8_t byteIndex = keycode / 8;
  uint8_t bitIndex  = keycode % 8;
  nkro_report[byteIndex] |= (1 << bitIndex);


  if (isModifierKey(keycode)) {
    uint8_t mod_bit = keycode - 0xE0;
    nkro_modifiers |= (1 << mod_bit);
  }
}

void releaseKeyNKRO(uint8_t keycode) {
  uint8_t byteIndex = keycode / 8;
  uint8_t bitIndex  = keycode % 8;
  nkro_report[byteIndex] &= ~(1 << bitIndex);


  if (isModifierKey(keycode)) {
    uint8_t mod_bit = keycode - 0xE0;
    nkro_modifiers &= ~(1 << mod_bit);
  }
}

void NKROReport(Adafruit_USBD_HID& usb_hid, uint8_t report_id) {
  uint8_t report[1 + NKRO_BYTES];       // 1 byte modifier + 13 bytes keys
  report[0] = nkro_modifiers;
  memcpy(report + 1, nkro_report, NKRO_BYTES);
  usb_hid.sendReport(report_id, report, sizeof(report));
  Serial.println("send nkro");
}

bool handleKeyboardNKRO(PinMapping &pin_mapping) {
    bool keys_changed = false;

    if (pin_mapping.is_just_pressed()) {
      pressKeyNKRO(pin_mapping.input_id);
      keys_changed = true;
    }
    else if (pin_mapping.is_just_released()) {
      releaseKeyNKRO(pin_mapping.input_id);
      keys_changed = true;
    }
    //update so that next iteration, we can detect press/unpress
    return keys_changed;
}