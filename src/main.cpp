#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include "nkro_keyboard.cpp"

#define DEBOUNCE_TICKS_MS 15


// Report ID
enum {
  NONE = 0,
  RID_KEYBOARD,
  RID_MOUSE,
  RID_GAMEPAD,
  RID_CONSUMER_CONTROL, // Media, volume etc ..
};

const uint8_t desc_hid_report[] = {
  TUD_HID_REPORT_DESC_NKRO_KEYBOARD (HID_REPORT_ID(RID_KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE         (HID_REPORT_ID(RID_MOUSE)),
  TUD_HID_REPORT_DESC_GAMEPAD       (HID_REPORT_ID(RID_GAMEPAD)),
  TUD_HID_REPORT_DESC_CONSUMER      (HID_REPORT_ID(RID_CONSUMER_CONTROL)),
};

enum {
  KEYBOARD = RID_KEYBOARD,
  MOUSE = RID_MOUSE,
  GAMEPAD_BUTTON = RID_GAMEPAD,
  CONSUMER = RID_CONSUMER_CONTROL,
  GAMEPAD_HAT,
  GAMEPAD_AXIS,
};

enum { 
  GAMEPAD_LEFT_ANALOG_X,
  GAMEPAD_LEFT_ANALOG_Y,
  GAMEPAD_LEFT_TRIGGER,
  GAMEPAD_RIGHT_ANALOG_X,
  GAMEPAD_RIGHT_ANALOG_Y,
  GAMEPAD_RIGHT_TRIGGER
};

//custom addition to match existing MOUSE_BUTTON_* enums
enum {
  MOUSE_SCROLL_UP     = TU_BIT(5),
  MOUSE_SCROLL_DOWN   = TU_BIT(6),
  MOUSE_SCROLL_LEFT   = TU_BIT(7),
  MOUSE_SCROLL_RIGHT  = TU_BIT(8),
  MOUSE_MOVE_UP       = TU_BIT(9),
  MOUSE_MOVE_DOWN     = TU_BIT(10),
  MOUSE_MOVE_LEFT     = TU_BIT(11),
  MOUSE_MOVE_RIGHT    = TU_BIT(12),
};

struct PinMapping {
  PinName pin_name = NC;
  int input_type = 0;
  u_int16_t input_id = 0; //this can be keycodes, consumer control keys, mouse buttons, or gamepad buttons
  
  int value = 0;
  int previous_value = 0;
  uint last_change_time = 0;
  
  bool analog = false;
  bool invert = false;
  //deadzone being/end?
  #define ADC_MAX_VALUE 1024
  int max_report_value = 256; //Gamepad descriptor is currently set to only use 8-bit axis, so the max report value is 256
  int activation_value = 30; 
  int center = ADC_MAX_VALUE / 2; //TODO allow calibrating this value dynamically
  int range = center; //TODO allow calibrating this value dynamically
  float scale = (float) max_report_value / (float) ADC_MAX_VALUE + 0.05; //TODO allow calibrating this value dynamically
  float deadzone_percent = 0.05; 
  int change_amount_before_update = ((float) ADC_MAX_VALUE * 0.005);

  
  bool quick_release = false; //TODO implement
  int counter_strafe_help_time_ms = 0; //TODO implement



  PinMapping(PinName pin = NC, int type = 0, u_int16_t id = 0, bool isAnalog = false, bool isInverted = false, 
    bool quickRelease = false, int counterStrafeHelpTime = 0)

      : pin_name(pin), input_type(type), input_id(id), analog(isAnalog), invert(isInverted), 
        //these are unimplemented
        quick_release(quickRelease), counter_strafe_help_time_ms(counterStrafeHelpTime),
        value(0), previous_value(0) {}
  
  void update_value() {
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



  int get_value_digital() {
    int value = digitalRead(pin_name);

    //TODO adjust this so that the "activated" state can be defined. I want 0 to always be not activated, and 1 to be activated, will similify some of the value == HIGH/LOW checks and multipliers

    if (invert) {
      value = 1 - value;
    }

    //aply scaling
    value = (int) ((float) value * scale);

    return value;
  }
  int get_value_analog() {
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
  int read_value() {
    if (analog) {
      return get_value_analog();
    }

    return get_value_digital();;
  }


  const int get_value() {
    return value;
  }

  const bool is_pressed(int test_value = -1) {
    //if no value is supplied, default to the current value
    if (test_value == -1) test_value = value;

    if (analog) {
      //TODO implement quick-release here
      return test_value >= activation_value;
    }

    return test_value == LOW;//TODO: change this to have scaling compatibility
  }
  const bool is_released(int test_value = -1) {
    //if no value is supplied, default to the current value
    if (test_value == -1) test_value = value;

    if (analog) {
      return not is_pressed(test_value);
    }

    return test_value == HIGH;//TODO: change this to have scaling compatibility
  }
  const bool is_just_pressed() {
    return is_pressed() and is_released(previous_value);
  }
  const bool is_just_released() {
    return is_released() and is_pressed(previous_value);
  }
  const bool is_just_changed() {
    return is_just_pressed() or is_just_released();
  }
  const bool is_bounce() {
    return (millis() - last_change_time) <= DEBOUNCE_TICKS_MS;
  }
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


hid_mouse_report_t mouse;
hid_gamepad_report_t gamepad;


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
    NKROReport(usb_hid, RID_KEYBOARD);
  }
  if (
    gamepad.buttons != last_gamepad.buttons or
    gamepad.hat != last_gamepad.hat or
    gamepad.x  != last_gamepad.x  or
    gamepad.y  != last_gamepad.y  //or
    //gamepad.z  != last_gamepad.z  or
    //gamepad.rz != last_gamepad.rz or
    //gamepad.rx != last_gamepad.rx or
    //gamepad.ry != last_gamepad.ry
  ) {
    usb_hid.sendReport(RID_GAMEPAD, &gamepad, sizeof(gamepad));
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
    usb_hid.sendReport(RID_MOUSE, &mouse, sizeof(mouse));
    Serial.println("send mouse");
  }
  //TODO: consumer control keys


  //we don't need to run at full speed. My measurement showed that iterations take ~43 microseconds. This could possible help avoid USB buffer issues, though idk if that's even a problem
  delayMicroseconds(457);
  //wdelay(50);
  //Serial.println(digitalRead(PB_15));
  //Serial.println(analogRead(PA_1));
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


