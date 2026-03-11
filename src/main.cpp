#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include "pin_mapping.h"
#include "nkro_keyboard.h"
#include "input_enums.h"
#include "input_handlers.h"



const uint8_t desc_hid_report[] = {
  TUD_HID_REPORT_DESC_NKRO_KEYBOARD (HID_REPORT_ID(KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE         (HID_REPORT_ID(MOUSE)),
  TUD_HID_REPORT_DESC_GAMEPAD       (HID_REPORT_ID(GAMEPAD)),
  TUD_HID_REPORT_DESC_CONSUMER      (HID_REPORT_ID(CONSUMER_CONTROL)),
};


PinMapping pins_in_use[] = {
    //thumb
    //TODO figure out how to add control rotation
    PinMapping(PA_0, KEYBOARD, HID_KEY_S, true, true),  //x (+3.3v forward)
    PinMapping(PA_0, KEYBOARD, HID_KEY_W, true, false),

    PinMapping(PA_1, KEYBOARD, HID_KEY_A, true, true),  //y (+3.3v up)
    PinMapping(PA_1, KEYBOARD, HID_KEY_D, true, false),

    PinMapping(PA_2, KEYBOARD, HID_KEY_1),  //click //broken atm

    //index
    PinMapping(PB_9, KEYBOARD, HID_KEY_1),  //tip
    PinMapping(PB_8, KEYBOARD, HID_KEY_R), //rest
    //PinMapping(PB_7, KEYBOARD, HID_KEY_R),  //inner
    //PinMapping(PB_6, GAMEPAD_BUTTON, GAMEPAD_BUTTON_B),  //pad
    
    //middle
    PinMapping(PB_5, KEYBOARD, HID_KEY_5), //tip
    //PinMapping(PB_4, KEYBOARD, HID_KEY_W),  //rest
    PinMapping(PB_3, KEYBOARD, HID_KEY_B),  //inner
    //PinMapping(PA_15, KEYBOARD, HID_KEY_X),  //pad
    
    //ring
    PinMapping(PA_10, KEYBOARD, HID_KEY_E),  //tip
    //qPinMapping(PA_9, KEYBOARD, HID_KEY_A),  //rest
    //PinMapping(PA_8, KEYBOARD, HID_KEY_Z),  //inner
    //PinMapping(PB_10, KEYBOARD, HID_KEY_ALT_LEFT), //pad
    
    //pinky
    PinMapping(PB_1, KEYBOARD, HID_KEY_G), //tip
    PinMapping(PB_0, KEYBOARD, HID_KEY_SPACE), //rest
    PinMapping(PA_7, KEYBOARD, HID_KEY_SHIFT_LEFT), //inner
    PinMapping(PA_6, KEYBOARD, HID_KEY_CONTROL_LEFT),  //pad
};


Adafruit_USBD_CDC usb_serial; //the adafruit tinyusb serial object must be used to retain both HID and serial communication

// USB HID device
Adafruit_USBD_HID usb_hid;





void setup() {
  usb_serial.begin(115200);
  delay(200);
  
  //HID setup  
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();
  
  
  //configure pinmode for button pins
  const size_t NUM_PINS = sizeof(pins_in_use) / sizeof(pins_in_use[0]);
  for (PinMapping &pin_mapping : pins_in_use) {
    pinMode(pin_mapping.pin_name, INPUT_PULLUP);
  }

  while (!TinyUSBDevice.mounted()) delay(100);
}





void loop() {
  if (not usb_hid.ready()) {
    //Serial.println("not ready");
    //delayMicroseconds(1000);
    return;
  }

  bool nkro_updated = false;

  hid_mouse_report_t last_mouse = mouse;
  //reset mouse inputs
  memset(&mouse, 0, sizeof(mouse));
  
  hid_gamepad_report_t last_gamepad = gamepad;
  //reset gamepad inputs
  memset(&gamepad, 0, sizeof(gamepad));
  gamepad.hat = GAMEPAD_HAT_CENTERED;

  for (PinMapping &pin_mapping : pins_in_use) {
    //if analog
    pin_mapping.update_value();

    switch (pin_mapping.input_type) {
      case KEYBOARD:
        nkro_updated = nkro_updated or handleKeyboardNKRO(pin_mapping);
        break;
      case MOUSE:
        mouse.buttons |= handleMouseButtons(pin_mapping);
        mouse.x += handleMouseMovementX(pin_mapping);
        mouse.y += handleMouseMovementY(pin_mapping);
        mouse.wheel += handleMouseScroll(pin_mapping);
        mouse.pan += handleMousePan(pin_mapping);
        break;
      case GAMEPAD_BUTTON:
        gamepad.buttons |= handleGamepadButtons(pin_mapping);
        break;
      case GAMEPAD_HAT:
        gamepad.hat = handleGamepadHat(pin_mapping);
        break;
      case GAMEPAD_AXIS:
        handleGamepadAxis(pin_mapping); //I think this might be a better convention than the previous ones. It will be easier to separate out into multiple files if needed
        break;
      //case RID_CONSUMER_CONTROL:
        //usb_hid.sendReport16(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_VOLUME_DECREMENT);
        //TODO
    }
  }

  if (nkro_updated) {
    NKROReport(usb_hid, KEYBOARD);
  }
  if (
    gamepad.buttons != last_gamepad.buttons or
    gamepad.hat != last_gamepad.hat or
    gamepad.x  != last_gamepad.x  or
    gamepad.y  != last_gamepad.y  or
    gamepad.z  != last_gamepad.z  or
    gamepad.rz != last_gamepad.rz or
    gamepad.rx != last_gamepad.rx or
    gamepad.ry != last_gamepad.ry
  ) {
    usb_hid.sendReport(GAMEPAD, &gamepad, sizeof(gamepad));
    Serial.println("send gamepad");
    //Serial.println(gamepad.x);
    //Serial.println(last_gamepad.x);
  }
  if (
    mouse.buttons != last_mouse.buttons or
    mouse.x != 0.0 or
    mouse.y != 0.0 or
    mouse.wheel != 0.0 or
    mouse.pan != 0.0
  ) {
    usb_hid.sendReport(MOUSE, &mouse, sizeof(mouse));
    Serial.println("send mouse");
  }
  //TODO: consumer control keys

  //we don't need to run at full speed. My measurement showed that iterations take ~43 microseconds. This could possible help avoid USB buffer issues, though idk if that's even a problem
  delayMicroseconds(457);
}









//ideal structure

//fast clock interrupt timer samples pins, if a pin has changed, add it to a queue
//do debounce logic here ^^

//allegedly, this is a fast way to sample pins:
//uint32_t new_state = GPIOx->IDR;
//changed = new_state ^ previous_state

//slower loop()/polling function then processes the state change queue, and calls out to a function pointer 
//to handle that specific pin (depending on config, if it's keyboard/mouse/gamepad or analog/digital)

//these pointer'd functions can only modify the current state (i.e. nkro report, gamepad report)

//at the end of the iteration, reports that have been changed will be reported to usbhid












//This is an example of an interrupt scanning
/*
#include <Arduino.h>

/* -----------------------------
   Shared state between ISR/loop
----------------------------- 

volatile bool scan_flag = false;   // set by ISR when scan should run

/* -----------------------------
   Timer ISR
-----------------------------

HardwareTimer *scanTimer = nullptr;

void scanISR() {
    scan_flag = true;   // signal main loop
}

/* -----------------------------
   Timer setup
----------------------------- 

void setupScanTimer(uint32_t frequency_hz)
{
    scanTimer = new HardwareTimer(TIM2);   // choose an available timer

    scanTimer->setOverflow(frequency_hz, HERTZ_FORMAT);  // e.g. 1000 Hz
    scanTimer->attachInterrupt(scanISR);
    scanTimer->resume();
}

/* -----------------------------
   Setup
-----------------------------

void setup() {
    Serial.begin(115200);

    setupScanTimer(1000);   // 1 kHz scan rate
}

/* -----------------------------
   Input scan (runs in loop)
-----------------------------

void scanInputs() {
    // this will eventually contain:
    // pin_mapping.update_value()
}

/* -----------------------------
   Main loop
-----------------------------

void loop() {

    if (scan_flag) {
        scan_flag = false;

        scanInputs();   // run input scan
    }

    // slower tasks go here
}
*/

//reset from code: NVIC_SystemReset();

//Serial.readString()

//HAL_FLASH_Unlock()
//HAL_FLASH_Program()
//HAL_FLASH_Erase()


