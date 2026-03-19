#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#include "pin_declaration.h"
#include "pin_mapping.h"
#include "input_augmentation.h"
#include "nkro_keyboard.h"
#include "input_enums.h"
#include "input_handlers.h"

const uint8_t desc_hid_report[] = {
  TUD_HID_REPORT_DESC_NKRO_KEYBOARD (HID_REPORT_ID(KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE         (HID_REPORT_ID(MOUSE)),
  TUD_HID_REPORT_DESC_GAMEPAD       (HID_REPORT_ID(GAMEPAD)),
  //TUD_HID_REPORT_DESC_CONSUMER      (HID_REPORT_ID(CONSUMER_CONTROL)),
};

//TODO replace with std::vector. reserve() and resize()
//Maybe this could be fixed by mapping pins to unique identifier names, like thumb_x or index_tip
std::map<uint, Pin> pin_map = {
  { PA_0, Pin(PA_0, true) },
  { PA_1, Pin(PA_1, true) },
  { PA_2, Pin(PA_2) },
  { PB_9, Pin(PB_9) },
  { PB_8, Pin(PB_8) },
  { PB_7, Pin(PB_7) },
  { PB_6, Pin(PB_6) },
  { PB_5, Pin(PB_5) },
  { PB_4, Pin(PB_4) },
  { PB_3, Pin(PB_3) },
  { PA_15, Pin(PA_15) },
  { PA_10, Pin(PA_10) },
  { PA_9, Pin(PA_9) },
  { PA_8, Pin(PA_8) },
  { PB_10, Pin(PB_10) },
  { PB_1, Pin(PB_1) },
  { PB_0, Pin(PB_0) },
  { PA_7, Pin(PA_7) },
  { PA_6, Pin(PA_6) },
};

//eh, low priority, as I think that typically Macros are abused to get an unfair advantage instead of being useful toons 99% of the time
//anyways, normal hotkeys can be done by binding multiple keys to a single pin
//Consider different config structures for input mappings, to accommodate things like macros
std::vector<PinMapping> pin_bindings = {
    //thumb
    PinMapping(PA_2, KEYBOARD, HID_KEY_CONTROL_LEFT),  //click
    PinMapping(PA_2, KEYBOARD, HID_KEY_ALT_LEFT),  //click
    PinMapping(PA_2, KEYBOARD, HID_KEY_M),  //click
    
    
    //TODO re-implement gamepad binding logic, as values centered at 0 don't work properly
    PinMapping(PA_1, KEYBOARD, HID_KEY_D, true, false),
    PinMapping(PA_0, KEYBOARD, HID_KEY_W, true, false),
    PinMapping(PA_1, KEYBOARD, HID_KEY_A, true, true),  //y (+3.3v up)
    PinMapping(PA_0, KEYBOARD, HID_KEY_S, true, true),  //x (+3.3v forward)
    
    PinMapping(PA_1, GAMEPAD_AXIS, GAMEPAD_LEFT_STICK_X, true, false),
    PinMapping(PA_0, GAMEPAD_AXIS, GAMEPAD_LEFT_STICK_Y, true, true),

    //index
    PinMapping(PB_9, KEYBOARD, HID_KEY_1),  //tip
    PinMapping(PB_8, KEYBOARD, HID_KEY_R), //rest
    PinMapping(PB_7, KEYBOARD, HID_KEY_NONE),  //inner
    PinMapping(PB_6, KEYBOARD, HID_KEY_NONE),  //pad
    
    //middle
    PinMapping(PB_5, KEYBOARD, HID_KEY_5), //tip
    PinMapping(PB_4, KEYBOARD, HID_KEY_NONE),  //rest
    PinMapping(PB_3, KEYBOARD, HID_KEY_B),  //inner
    PinMapping(PA_15, KEYBOARD, HID_KEY_NONE),  //pad
    
    //ring
    PinMapping(PA_10, KEYBOARD, HID_KEY_E),  //tip
    PinMapping(PA_9, KEYBOARD, HID_KEY_NONE),  //rest
    PinMapping(PA_8, KEYBOARD, HID_KEY_NONE),  //inner
    PinMapping(PB_10, KEYBOARD, HID_KEY_NONE), //pad
    
    //pinky
    PinMapping(PB_1, KEYBOARD, HID_KEY_G), //tip
    PinMapping(PB_0, KEYBOARD, HID_KEY_SPACE), //rest
    PinMapping(PA_7, KEYBOARD, HID_KEY_SHIFT_LEFT), //inner
    PinMapping(PA_6, KEYBOARD, HID_KEY_CONTROL_LEFT),  //pad
};

std::vector<InputAugmentation> augmentations = {
  InputAugmentation::rotation(&pin_map[PA_0], &pin_map[PA_1], radians(-0.0f)),
  InputAugmentation::rotation(&pin_map[PA_1], &pin_map[PA_0], radians(-0.0f)),
};


Adafruit_USBD_CDC usb_serial; //the adafruit tinyusb serial object must be used to retain both HID and serial communication

// USB HID device
Adafruit_USBD_HID usb_hid;




const std::vector<String> split_string(String input, char delimiter = ' ') {
  std::vector<String> parameters = {};
  int last_index = 0;
  int index = 0;
  while(index != -1) {
    index = input.indexOf(delimiter, last_index);

    parameters.push_back(input.substring(last_index, index != -1 ? index : input.length()));

    last_index = index + 1;
  }
  return parameters;
}

enum Commands{
  InputMapping,
  PinDeclaration,
  Augmentation,
  ClearMap,
  ClearPin,
  ClearAugments,
};

void parse_line() {
  //read one line of incoming serial
  String input = Serial.readStringUntil('\n');
  if(input.length() < 3) return;

  String command_string = input.substring(0, 3);
  int command = -1;

  //TODO should these commands, map, pin, aug, also have a parameter for create/edit/remove?
  if (command_string.equals("map")) { //this refers to pin bindings. TODO sort out verbiage
    Serial.println("Received command to set mapping");
    command = InputMapping;
  } else if (command_string.equals("pin")) { //this refers to the pin map/pin declarations
    Serial.println("Received command to set pin declarations");
    command = PinDeclaration;
  } else if (command_string.equals("aug")) { //this refers to input augmentations
    Serial.println("Received command to set augmentation");
    command = Augmentation;
  } else if (command_string.equals("clm")) { //this refers to input augmentations
    Serial.println("Received command to clear mapping");
    command = ClearMap;
  } else if (command_string.equals("clp")) { //this refers to input augmentations
    Serial.println("Received command to clear pin declarations");
    command = ClearPin;
  } else if (command_string.equals("cla")) { //this refers to input augmentations
    Serial.println("Received command to clear augmentations");
    command = ClearAugments;
  }

  if (command == -1) return; //no command parsed

  //split input into arguments, delimited by space
  std::vector<String> arguments = split_string(input);
  
  //TODO instead of using text for this, configs could be sent directly in byte format to avoid serial > string > typecast complexity.
  switch (command)
  {
  case InputMapping:
    if (arguments.size() != 6) {
      Serial.println("Wrong number of arguments supplied for command");
      return;
    }
    
    //expected command syntax: map 2 1 30 0 0
    // command, pin number, input device, input id, analog, invert
    pin_bindings[0] = PinMapping((PinName) arguments[1].toInt(), arguments[2].toInt(), arguments[3].toInt(), arguments[4].toInt(), arguments[5].toInt());
    Serial.println("Pin number: " + (String) (PinName) arguments[1].toInt() + " Input device: " + (String) arguments[2].toInt() + " Input id: " + (String) arguments[3].toInt() + " Analog: " + (String) arguments[4].toInt() + " Inverted: " + (String) arguments[5].toInt());
    break;
  case PinDeclaration:
    if (arguments.size() != 4) {
      Serial.println("Wrong number of arguments supplied for command");
      return;
    }
    //only for digital pins atm
    //expected command syntax pin 2 1
    //pin, inactive_value)
    pin_map[arguments[1].toInt()] = Pin((PinName) arguments[1].toInt(), arguments[3].toInt());
    break;
  case Augmentation:
    /* code */
    //TODO
    break;
  case ClearMap:
    //clear mappings
    pin_bindings.clear();
    //not sure if I should actually deallocate memory, so i'll keep it simple for now.
    //pin_bindings.shrink_to_fit();
    break;
  case ClearPin:
    //clear pin declarations
    pin_map.clear();
    break;
  case ClearAugments:
    //clear augments
    augmentations.clear();
    //not sure if I should actually deallocate memory, so i'll keep it simple for now.
    //augmentations.shrink_to_fit();
    break;
  }
}

void parse_serial() {
  while (Serial.available()) {
    parse_line();
  }
}









void setup() {
  usb_serial.begin(115200);
  usb_serial.setTimeout(100); //timeout after 0.1 seconds
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

  delay(50);
  
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
  //TODO this absolutely 100% must only apply to pin readings, not pin mappings
  for (auto& augment : augmentations) {
    augment.apply_augmentations();
  }

  for (auto& binding : pin_bindings) {
    binding.update_value();
  }

  //Serial.println(pin_bindings[0].is_just_pressed());
  //Serial.println(pin_bindings[1].is_just_pressed());
  //Serial.println(pin_bindings[2].is_just_pressed());


  
  //Serial.print("M:");
  //Serial.print(nkro_report[HID_KEY_M / 8] & (1 << (HID_KEY_M % 8)));
//
  //Serial.print(" CTRL:");
  //Serial.print(nkro_modifiers & 0x02);
//
  //Serial.print(" SHIFT:");
  //Serial.println(nkro_modifiers & 0x01);
  
  
  //Serial.println();
  //Serial.println((String) pin_bindings[1].value + ", " + (String) pin_bindings[1].previous_value);
  //Serial.println((String) pin_bindings[0].is_pressed() + ", " + (String) pin_bindings[0].is_released(pin_bindings[0].previous_value));
  

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
        //TODO
    }
  }

  if (nkro_updated) {
    //TODO fix the reporting logic for this, possibly even look into a queue system 
    
    
    //Serial.print("mods: ");
    //Serial.println(nkro_modifiers, BIN);
//
    //for (int i = 0; i < NKRO_BYTES; i++) {
    //  Serial.print((String) nkro_report[i] + ", ");
    //}
    //Serial.println();


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
    //TODO track down noise issues with this to avoid spam
  ) {
    //usb_hid.sendReport(GAMEPAD, &gamepad, sizeof(gamepad));
    //Serial.println("send gamepad");
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


