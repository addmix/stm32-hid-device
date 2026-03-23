#pragma once
#include "pin_declaration.h"
#include "pin_mapping.h"
#include "input_augmentation.h"
#include "input_enums.h"

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
    
    //thumbstick
    PinMapping(PA_1, KEYBOARD, HID_KEY_D, false),
    PinMapping(PA_0, KEYBOARD, HID_KEY_W, false),
    PinMapping(PA_1, KEYBOARD, HID_KEY_A, true),  //y (+3.3v up)
    PinMapping(PA_0, KEYBOARD, HID_KEY_S, true),  //x (+3.3v forward)
    
    PinMapping(PA_1, GAMEPAD_AXIS, GAMEPAD_LEFT_STICK_X, false),
    PinMapping(PA_0, GAMEPAD_AXIS, GAMEPAD_LEFT_STICK_Y, true),

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
  InputAugmentation::rotation(PA_0, PA_1, radians(-0.0f)),
  InputAugmentation::rotation(PA_1, PA_0, radians(-0.0f)),
};
