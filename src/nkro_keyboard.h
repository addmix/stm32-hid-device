#pragma once
#include <Adafruit_TinyUSB.h>
#include "pin_mapping.h"
#include "input_enums.h"

//provides functionality/mapping for NKRO (next-key roll over) keyboard reporting. Adafruit TinyUSB's default keyboard functions only allow 6 simultaneous keypresses.

//theoretical maximum for nkro (232 keys)
#define NKRO_BYTES 29
extern uint8_t nkro_report[NKRO_BYTES];
extern uint8_t nkro_modifiers;

#define TUD_HID_REPORT_DESC_NKRO_KEYBOARD(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     ),\
  HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD ),\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION ),\
    /* Optional Report ID or extra descriptors */ \
    __VA_ARGS__ \
\
    /* 8 modifier keys (0xE0–0xE7) */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,\
      HID_USAGE_MIN    ( 224                                    )  ,\
      HID_USAGE_MAX    ( 231                                    )  ,\
      HID_LOGICAL_MIN  ( 0                                      )  ,\
      HID_LOGICAL_MAX  ( 1                                      )  ,\
      HID_REPORT_COUNT ( 8                                      )  ,\
      HID_REPORT_SIZE  ( 1                                      )  ,\
      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )  ,\
\
    /* Full keyboard NKRO bitmap (0x00–0xE7) */ \
      HID_USAGE_MIN    ( 0                                      )  ,\
      HID_USAGE_MAX_N  ( 231, 2                                 )  ,\
      HID_LOGICAL_MIN  ( 0                                      )  ,\
      HID_LOGICAL_MAX  ( 1                                      )  ,\
      HID_REPORT_COUNT ( 232                                    )  ,\
      HID_REPORT_SIZE  ( 1                                      )  ,\
      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )  ,\
\
  HID_COLLECTION_END


bool isModifierKey(uint8_t keycode);
void pressKeyNKRO(uint8_t keycode);
void releaseKeyNKRO(uint8_t keycode);
void NKROReport(Adafruit_USBD_HID& usb_hid, uint8_t report_id);
bool handleKeyboardNKRO(PinMapping &pin_mapping);