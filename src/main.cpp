#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#include "pin_declaration.h"
#include "pin_mapping.h"
#include "input_augmentation.h"
#include "nkro_keyboard.h"
#include "input_enums.h"
#include "input_handlers.h"
#include "serialization.h"
#include "config.h"

const uint8_t desc_hid_report[] = {
  TUD_HID_REPORT_DESC_NKRO_KEYBOARD (HID_REPORT_ID(KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE         (HID_REPORT_ID(MOUSE)),
  TUD_HID_REPORT_DESC_GAMEPAD       (HID_REPORT_ID(GAMEPAD)),
  //TUD_HID_REPORT_DESC_CONSUMER      (HID_REPORT_ID(CONSUMER_CONTROL)),
};

Adafruit_USBD_CDC usb_serial; //the adafruit tinyusb serial object must be used to retain both HID and serial communication
Adafruit_USBD_HID usb_hid;

void setup() {
  usb_serial.begin(115200);
  usb_serial.setTimeout(2000); //timeout after 0.1 seconds
  //delay(50);
  
  //HID setup  
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();
  
  //setup pins for pullup/pulldown
  for (const auto& [key, pin] : pin_map) {
    if (pin.analog) {
      pinMode(pin.pin_name, INPUT_ANALOG);
      continue;
    }
    if (pin.inactive_value == LOW) pinMode(pin.pin_name, INPUT_PULLDOWN);
    else pinMode(pin.pin_name, INPUT_PULLUP);
  }

  while (!TinyUSBDevice.mounted()) delay(100);
}

void loop() {
  if (not usb_hid.ready()) {
    //Serial.println("not ready");
    //delayMicroseconds(1000);
    return;
  }
  //delay(50);
  //run parsing routine
  parse_serial();


  //TODO
  //TODO Change this to a queue/buffer based implementation
  //TODO

  //update all pin readings as concurrently as possible
  for (auto& [key, pin] : pin_map) {
    pin.update();
  }

  //apply input augmentations
  for (auto& augment : augmentations) {
    augment.apply_augmentations();
  }

  for (auto& binding : pin_bindings) {
    binding.update_value();
  }

  bool nkro_updated = false;

  hid_mouse_report_t last_mouse = mouse;
  //reset mouse inputs
  memset(&mouse, 0, sizeof(mouse));
  
  hid_gamepad_report_t last_gamepad = gamepad;
  //reset gamepad inputs
  memset(&gamepad, 0, sizeof(gamepad));
  gamepad.hat = GAMEPAD_HAT_CENTERED;

  for (PinMapping &pin_binding : pin_bindings) {
    switch (pin_binding.input_type) {
      case KEYBOARD:
        //TODO remove this "nkro_updated" logic, and just check if the report is different at the end of the iteration
        nkro_updated = handleKeyboardNKRO(pin_binding) or nkro_updated; //this ordering is necessary to prevent nkro_updated from short-circuiting and preventing the function call
        break;
      case MOUSE:
      //TODO: Simplify this to be more like the gamepad axis handler
        mouse.buttons |= handleMouseButtons(pin_binding);
        mouse.x += handleMouseMovementX(pin_binding);
        mouse.y += handleMouseMovementY(pin_binding);
        mouse.wheel += handleMouseScroll(pin_binding);
        mouse.pan += handleMousePan(pin_binding);
        break;
      case GAMEPAD_BUTTON:
      //TODO: Simplify this to be more like the gamepad axis handler
        gamepad.buttons |= handleGamepadButtons(pin_binding);
        break;
      case GAMEPAD_HAT:
      //TODO: Simplify this to be more like the gamepad axis handler
        gamepad.hat = handleGamepadHat(pin_binding);
        break;
      case GAMEPAD_AXIS:
        handleGamepadAxis(pin_binding); //I think this might be a better convention than the previous ones. It will be easier to separate out into multiple files if needed
        break;
      //case RID_CONSUMER_CONTROL:
        //usb_hid.sendReport16(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_VOLUME_DECREMENT);
    }
  }

  if (nkro_updated) {
    //TODO fix the reporting logic for this
    NKROReport(usb_hid, KEYBOARD);
    Serial.println("send nkro");
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
    // pin_bindingsping.update_value()
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


