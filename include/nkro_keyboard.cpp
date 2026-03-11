#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

//provides functionality/mapping for NKRO (next-key roll over) keyboard reporting. Adafruit TinyUSB's default keyboard functions only allow 6 simultaneous keypresses.


//theoretical maximum for nkro (232 keys)
#define NKRO_BYTES 29
uint8_t nkro_report[NKRO_BYTES];
uint8_t nkro_modifiers = 0;

//#define TUD_HID_REPORT_DESC_NKRO_KEYBOARD(...) \
//  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     ),\
//  HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD ),\
//  HID_COLLECTION ( HID_COLLECTION_APPLICATION ),\
//    /* Optional Report ID or extra descriptors */ \
//    __VA_ARGS__ \
//    /* 8 bits Modifier Keys (Shift, Control, Alt) */ \
//    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,\
//      HID_USAGE_MIN    ( 224                                    )  ,\
//      HID_USAGE_MAX    ( 231                                    )  ,\
//      HID_LOGICAL_MIN  ( 0                                      )  ,\
//      HID_LOGICAL_MAX  ( 1                                      )  ,\
//      HID_REPORT_COUNT ( 8                                      )  ,\
//      HID_REPORT_SIZE  ( 1                                      )  ,\
//      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )  ,\
//    /* 104 keys bitmap (NKRO) */ \
//      HID_USAGE_MIN    ( 0                                      )  ,\
//      HID_USAGE_MAX_N  ( 103, 2                                 )  ,\
//      HID_LOGICAL_MIN  ( 0                                      )  ,\
//      HID_LOGICAL_MAX  ( 1                                      )  ,\
//      HID_REPORT_COUNT ( 104                                    )  ,\
//      HID_REPORT_SIZE  ( 1                                      )  ,\
//      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )  ,\
//  HID_COLLECTION_END

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

