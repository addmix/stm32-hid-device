class_name Enums

static func value_to_key(_enum : Dictionary, value : Variant) -> Variant:
	var index : int = _enum.values().find(value)
	if index == -1:
		return null
	
	return _enum.keys()[index]

static func text_to_index(text : String) -> int:
	var index : int = 255
	if text in ["", "-1"]:
		index = 255
	else:
		if text.is_valid_int(): index = text.to_int() #if value isn't proper int, the default of 255 is sent
	
	return index

static func text_to_pin(text : String) -> int:
	if Pins.has(text):
		return Pins.get(text)
	
	return text.to_int()

static func pin_to_text(pin : int) -> String:
	return value_to_key(Pins, pin)

static func text_to_input_id(text : String, device_type : int = Device.KEYBOARD) -> int:
	match device_type:
		Device.KEYBOARD: #defaults to keyboard if no device_type is passed
			return Keyboard.get(text)
		Device.MOUSE:
			return Mouse.get(text)
		Device.GAMEPAD_BUTTON:
			return GamepadAxis.get(text)
		Device.GAMEPAD_HAT:
			pass #todo
		Device.GAMEPAD_AXIS :
			pass #todo
	
	return -1

static func input_id_to_text(input_id : int, device_type : int = Device.KEYBOARD) -> String:
	match device_type:
		Device.KEYBOARD: #defaults to keyboard if no device_type is passed
			return value_to_key(Keyboard, input_id)
		Device.MOUSE:
			return value_to_key(Mouse, input_id)
		Device.GAMEPAD_BUTTON:
			return value_to_key(GamepadAxis, input_id)
		Device.GAMEPAD_HAT:
			pass #todo
		Device.GAMEPAD_AXIS :
			pass #todo
	
	return ""

enum Commands {
	Reset,
	TextCommand,
	GetConfigs,
	GetPins,
	GetMappings,
	GetAugments,
	SetConfigs,
	SetPins,
	SetMappings,
	SetAugments,
	ListConfigs,
	ListPins,
	ListMappings,
	ListAugments,
	ClearConfigs,
	ClearPin,
	ClearMap,
	ClearAugments,
	#adds/edits items (depending on type)
	PinDeclaration,
	InputMapping,
	Augmentation,
}

enum Device{
  KEYBOARD = 1,
  MOUSE,
  GAMEPAD,
  GAMEPAD_BUTTON,
  GAMEPAD_HAT,
  GAMEPAD_AXIS, 
}

#typedef enum
#{
  #GAMEPAD_BUTTON_0  = TU_BIT(0),
  #GAMEPAD_BUTTON_1  = TU_BIT(1),
  #GAMEPAD_BUTTON_2  = TU_BIT(2),
  #GAMEPAD_BUTTON_3  = TU_BIT(3),
  #GAMEPAD_BUTTON_4  = TU_BIT(4),
  #GAMEPAD_BUTTON_5  = TU_BIT(5),
  #GAMEPAD_BUTTON_6  = TU_BIT(6),
  #GAMEPAD_BUTTON_7  = TU_BIT(7),
  #GAMEPAD_BUTTON_8  = TU_BIT(8),
  #GAMEPAD_BUTTON_9  = TU_BIT(9),
  #GAMEPAD_BUTTON_10 = TU_BIT(10),
  #GAMEPAD_BUTTON_11 = TU_BIT(11),
  #GAMEPAD_BUTTON_12 = TU_BIT(12),
  #GAMEPAD_BUTTON_13 = TU_BIT(13),
  #GAMEPAD_BUTTON_14 = TU_BIT(14),
  #GAMEPAD_BUTTON_15 = TU_BIT(15),
  #GAMEPAD_BUTTON_16 = TU_BIT(16),
  #GAMEPAD_BUTTON_17 = TU_BIT(17),
  #GAMEPAD_BUTTON_18 = TU_BIT(18),
  #GAMEPAD_BUTTON_19 = TU_BIT(19),
  #GAMEPAD_BUTTON_20 = TU_BIT(20),
  #GAMEPAD_BUTTON_21 = TU_BIT(21),
  #GAMEPAD_BUTTON_22 = TU_BIT(22),
  #GAMEPAD_BUTTON_23 = TU_BIT(23),
  #GAMEPAD_BUTTON_24 = TU_BIT(24),
  #GAMEPAD_BUTTON_25 = TU_BIT(25),
  #GAMEPAD_BUTTON_26 = TU_BIT(26),
  #GAMEPAD_BUTTON_27 = TU_BIT(27),
  #GAMEPAD_BUTTON_28 = TU_BIT(28),
  #GAMEPAD_BUTTON_29 = TU_BIT(29),
  #GAMEPAD_BUTTON_30 = TU_BIT(30),
  #GAMEPAD_BUTTON_31 = TU_BIT(31),
#}hid_gamepad_button_bm_t;
#
#/// Standard Gamepad Buttons Naming from Linux input event codes
#/// https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h
##define GAMEPAD_BUTTON_A       GAMEPAD_BUTTON_0
##define GAMEPAD_BUTTON_SOUTH   GAMEPAD_BUTTON_0
#
##define GAMEPAD_BUTTON_B       GAMEPAD_BUTTON_1
##define GAMEPAD_BUTTON_EAST    GAMEPAD_BUTTON_1
#
##define GAMEPAD_BUTTON_C       GAMEPAD_BUTTON_2
#
##define GAMEPAD_BUTTON_X       GAMEPAD_BUTTON_3
##define GAMEPAD_BUTTON_NORTH   GAMEPAD_BUTTON_3
#
##define GAMEPAD_BUTTON_Y       GAMEPAD_BUTTON_4
##define GAMEPAD_BUTTON_WEST    GAMEPAD_BUTTON_4
#
##define GAMEPAD_BUTTON_Z       GAMEPAD_BUTTON_5
##define GAMEPAD_BUTTON_TL      GAMEPAD_BUTTON_6
##define GAMEPAD_BUTTON_TR      GAMEPAD_BUTTON_7
##define GAMEPAD_BUTTON_TL2     GAMEPAD_BUTTON_8
##define GAMEPAD_BUTTON_TR2     GAMEPAD_BUTTON_9
##define GAMEPAD_BUTTON_SELECT  GAMEPAD_BUTTON_10
##define GAMEPAD_BUTTON_START   GAMEPAD_BUTTON_11
##define GAMEPAD_BUTTON_MODE    GAMEPAD_BUTTON_12
##define GAMEPAD_BUTTON_THUMBL  GAMEPAD_BUTTON_13
##define GAMEPAD_BUTTON_THUMBR  GAMEPAD_BUTTON_14
#
#/// Standard Gamepad HAT/DPAD Buttons (from Linux input event codes)
#typedef enum
#{
  #GAMEPAD_HAT_CENTERED   = 0,  ///< DPAD_CENTERED
  #GAMEPAD_HAT_UP         = 1,  ///< DPAD_UP
  #GAMEPAD_HAT_UP_RIGHT   = 2,  ///< DPAD_UP_RIGHT
  #GAMEPAD_HAT_RIGHT      = 3,  ///< DPAD_RIGHT
  #GAMEPAD_HAT_DOWN_RIGHT = 4,  ///< DPAD_DOWN_RIGHT
  #GAMEPAD_HAT_DOWN       = 5,  ///< DPAD_DOWN
  #GAMEPAD_HAT_DOWN_LEFT  = 6,  ///< DPAD_DOWN_LEFT
  #GAMEPAD_HAT_LEFT       = 7,  ///< DPAD_LEFT
  #GAMEPAD_HAT_UP_LEFT    = 8,  ///< DPAD_UP_LEFT

enum GamepadAxis{ 
  GAMEPAD_LEFT_STICK_X,
  GAMEPAD_LEFT_STICK_Y,
  GAMEPAD_LEFT_TRIGGER,
  GAMEPAD_RIGHT_STICK_X,
  GAMEPAD_RIGHT_STICK_Y,
  GAMEPAD_RIGHT_TRIGGER,
};

enum Mouse {
	MOUSE_BUTTON_LEFT     = 1 << 0,
	MOUSE_BUTTON_RIGHT    = 1 << 1,
	MOUSE_BUTTON_MIDDLE   = 1 << 2,
	MOUSE_BUTTON_BACKWARD = 1 << 3,
	MOUSE_BUTTON_FORWARD  = 1 << 4,
	
	SCROLL_UP     = 1 << 5,
	SCROLL_DOWN   = 1 << 6,
	SCROLL_LEFT   = 1 << 7,
	SCROLL_RIGHT  = 1 << 8,
	MOUSE_UP       = 1 << 9,
	MOUSE_DOWN     = 1 << 10,
	MOUSE_LEFT     = 1 << 11,
	MOUSE_RIGHT    = 1 << 12,
};

enum Keyboard {
	HID_KEY_NONE                        = 0x00,
	HID_KEY_A                           = 0x04,
	HID_KEY_B                           = 0x05,
	HID_KEY_C                           = 0x06,
	HID_KEY_D                           = 0x07,
	HID_KEY_E                           = 0x08,
	HID_KEY_F                           = 0x09,
	HID_KEY_G                           = 0x0A,
	HID_KEY_H                           = 0x0B,
	HID_KEY_I                           = 0x0C,
	HID_KEY_J                           = 0x0D,
	HID_KEY_K                           = 0x0E,
	HID_KEY_L                           = 0x0F,
	HID_KEY_M                           = 0x10,
	HID_KEY_N                           = 0x11,
	HID_KEY_O                           = 0x12,
	HID_KEY_P                           = 0x13,
	HID_KEY_Q                           = 0x14,
	HID_KEY_R                           = 0x15,
	HID_KEY_S                           = 0x16,
	HID_KEY_T                           = 0x17,
	HID_KEY_U                           = 0x18,
	HID_KEY_V                           = 0x19,
	HID_KEY_W                           = 0x1A,
	HID_KEY_X                           = 0x1B,
	HID_KEY_Y                           = 0x1C,
	HID_KEY_Z                           = 0x1D,
	HID_KEY_1                           = 0x1E,
	HID_KEY_2                           = 0x1F,
	HID_KEY_3                           = 0x20,
	HID_KEY_4                           = 0x21,
	HID_KEY_5                           = 0x22,
	HID_KEY_6                           = 0x23,
	HID_KEY_7                           = 0x24,
	HID_KEY_8                           = 0x25,
	HID_KEY_9                           = 0x26,
	HID_KEY_0                           = 0x27,
	HID_KEY_ENTER                       = 0x28,
	HID_KEY_ESCAPE                      = 0x29,
	HID_KEY_BACKSPACE                   = 0x2A,
	HID_KEY_TAB                         = 0x2B,
	HID_KEY_SPACE                       = 0x2C,
	HID_KEY_MINUS                       = 0x2D,
	HID_KEY_EQUAL                       = 0x2E,
	HID_KEY_BRACKET_LEFT                = 0x2F,
	HID_KEY_BRACKET_RIGHT               = 0x30,
	HID_KEY_BACKSLASH                   = 0x31,
	HID_KEY_EUROPE_1                    = 0x32,
	HID_KEY_SEMICOLON                   = 0x33,
	HID_KEY_APOSTROPHE                  = 0x34,
	HID_KEY_GRAVE                       = 0x35,
	HID_KEY_COMMA                       = 0x36,
	HID_KEY_PERIOD                      = 0x37,
	HID_KEY_SLASH                       = 0x38,
	HID_KEY_CAPS_LOCK                   = 0x39,
	HID_KEY_F1                          = 0x3A,
	HID_KEY_F2                          = 0x3B,
	HID_KEY_F3                          = 0x3C,
	HID_KEY_F4                          = 0x3D,
	HID_KEY_F5                          = 0x3E,
	HID_KEY_F6                          = 0x3F,
	HID_KEY_F7                          = 0x40,
	HID_KEY_F8                          = 0x41,
	HID_KEY_F9                          = 0x42,
	HID_KEY_F10                         = 0x43,
	HID_KEY_F11                         = 0x44,
	HID_KEY_F12                         = 0x45,
	HID_KEY_PRINT_SCREEN                = 0x46,
	HID_KEY_SCROLL_LOCK                 = 0x47,
	HID_KEY_PAUSE                       = 0x48,
	HID_KEY_INSERT                      = 0x49,
	HID_KEY_HOME                        = 0x4A,
	HID_KEY_PAGE_UP                     = 0x4B,
	HID_KEY_DELETE                      = 0x4C,
	HID_KEY_END                         = 0x4D,
	HID_KEY_PAGE_DOWN                   = 0x4E,
	HID_KEY_ARROW_RIGHT                 = 0x4F,
	HID_KEY_ARROW_LEFT                  = 0x50,
	HID_KEY_ARROW_DOWN                  = 0x51,
	HID_KEY_ARROW_UP                    = 0x52,
	HID_KEY_NUM_LOCK                    = 0x53,
	HID_KEY_KEYPAD_DIVIDE               = 0x54,
	HID_KEY_KEYPAD_MULTIPLY             = 0x55,
	HID_KEY_KEYPAD_SUBTRACT             = 0x56,
	HID_KEY_KEYPAD_ADD                  = 0x57,
	HID_KEY_KEYPAD_ENTER                = 0x58,
	HID_KEY_KEYPAD_1                    = 0x59,
	HID_KEY_KEYPAD_2                    = 0x5A,
	HID_KEY_KEYPAD_3                    = 0x5B,
	HID_KEY_KEYPAD_4                    = 0x5C,
	HID_KEY_KEYPAD_5                    = 0x5D,
	HID_KEY_KEYPAD_6                    = 0x5E,
	HID_KEY_KEYPAD_7                    = 0x5F,
	HID_KEY_KEYPAD_8                    = 0x60,
	HID_KEY_KEYPAD_9                    = 0x61,
	HID_KEY_KEYPAD_0                    = 0x62,
	HID_KEY_KEYPAD_DECIMAL              = 0x63,
	HID_KEY_EUROPE_2                    = 0x64,
	HID_KEY_APPLICATION                 = 0x65,
	HID_KEY_POWER                       = 0x66,
	HID_KEY_KEYPAD_EQUAL                = 0x67,
	HID_KEY_F13                         = 0x68,
	HID_KEY_F14                         = 0x69,
	HID_KEY_F15                         = 0x6A,
	HID_KEY_F16                         = 0x6B,
	HID_KEY_F17                         = 0x6C,
	HID_KEY_F18                         = 0x6D,
	HID_KEY_F19                         = 0x6E,
	HID_KEY_F20                         = 0x6F,
	HID_KEY_F21                         = 0x70,
	HID_KEY_F22                         = 0x71,
	HID_KEY_F23                         = 0x72,
	HID_KEY_F24                         = 0x73,
	HID_KEY_EXECUTE                     = 0x74,
	HID_KEY_HELP                        = 0x75,
	HID_KEY_MENU                        = 0x76,
	HID_KEY_SELECT                      = 0x77,
	HID_KEY_STOP                        = 0x78,
	HID_KEY_AGAIN                       = 0x79,
	HID_KEY_UNDO                        = 0x7A,
	HID_KEY_CUT                         = 0x7B,
	HID_KEY_COPY                        = 0x7C,
	HID_KEY_PASTE                       = 0x7D,
	HID_KEY_FIND                        = 0x7E,
	HID_KEY_MUTE                        = 0x7F,
	HID_KEY_VOLUME_UP                   = 0x80,
	HID_KEY_VOLUME_DOWN                 = 0x81,
	HID_KEY_LOCKING_CAPS_LOCK           = 0x82,
	HID_KEY_LOCKING_NUM_LOCK            = 0x83,
	HID_KEY_LOCKING_SCROLL_LOCK         = 0x84,
	HID_KEY_KEYPAD_COMMA                = 0x85,
	HID_KEY_KEYPAD_EQUAL_SIGN           = 0x86,
	HID_KEY_KANJI1                      = 0x87,
	HID_KEY_KANJI2                      = 0x88,
	HID_KEY_KANJI3                      = 0x89,
	HID_KEY_KANJI4                      = 0x8A,
	HID_KEY_KANJI5                      = 0x8B,
	HID_KEY_KANJI6                      = 0x8C,
	HID_KEY_KANJI7                      = 0x8D,
	HID_KEY_KANJI8                      = 0x8E,
	HID_KEY_KANJI9                      = 0x8F,
	HID_KEY_LANG1                       = 0x90,
	HID_KEY_LANG2                       = 0x91,
	HID_KEY_LANG3                       = 0x92,
	HID_KEY_LANG4                       = 0x93,
	HID_KEY_LANG5                       = 0x94,
	HID_KEY_LANG6                       = 0x95,
	HID_KEY_LANG7                       = 0x96,
	HID_KEY_LANG8                       = 0x97,
	HID_KEY_LANG9                       = 0x98,
	HID_KEY_ALTERNATE_ERASE             = 0x99,
	HID_KEY_SYSREQ_ATTENTION            = 0x9A,
	HID_KEY_CANCEL                      = 0x9B,
	HID_KEY_CLEAR                       = 0x9C,
	HID_KEY_PRIOR                       = 0x9D,
	HID_KEY_RETURN                      = 0x9E,
	HID_KEY_SEPARATOR                   = 0x9F,
	HID_KEY_OUT                         = 0xA0,
	HID_KEY_OPER                        = 0xA1,
	HID_KEY_CLEAR_AGAIN                 = 0xA2,
	HID_KEY_CRSEL_PROPS                 = 0xA3,
	HID_KEY_EXSEL                       = 0xA4,
	#RESERVED				            = 0xA5-AF,
	HID_KEY_KEYPAD_00                   = 0xB0,
	HID_KEY_KEYPAD_000                  = 0xB1,
	HID_KEY_THOUSANDS_SEPARATOR         = 0xB2,
	HID_KEY_DECIMAL_SEPARATOR           = 0xB3,
	HID_KEY_CURRENCY_UNIT               = 0xB4,
	HID_KEY_CURRENCY_SUBUNIT            = 0xB5,
	HID_KEY_KEYPAD_LEFT_PARENTHESIS     = 0xB6,
	HID_KEY_KEYPAD_RIGHT_PARENTHESIS    = 0xB7,
	HID_KEY_KEYPAD_LEFT_BRACE           = 0xB8,
	HID_KEY_KEYPAD_RIGHT_BRACE          = 0xB9,
	HID_KEY_KEYPAD_TAB                  = 0xBA,
	HID_KEY_KEYPAD_BACKSPACE            = 0xBB,
	HID_KEY_KEYPAD_A                    = 0xBC,
	HID_KEY_KEYPAD_B                    = 0xBD,
	HID_KEY_KEYPAD_C                    = 0xBE,
	HID_KEY_KEYPAD_D                    = 0xBF,
	HID_KEY_KEYPAD_E                    = 0xC0,
	HID_KEY_KEYPAD_F                    = 0xC1,
	HID_KEY_KEYPAD_XOR                  = 0xC2,
	HID_KEY_KEYPAD_CARET                = 0xC3,
	HID_KEY_KEYPAD_PERCENT              = 0xC4,
	HID_KEY_KEYPAD_LESS_THAN            = 0xC5,
	HID_KEY_KEYPAD_GREATER_THAN         = 0xC6,
	HID_KEY_KEYPAD_AMPERSAND            = 0xC7,
	HID_KEY_KEYPAD_DOUBLE_AMPERSAND     = 0xC8,
	HID_KEY_KEYPAD_VERTICAL_BAR         = 0xC9,
	HID_KEY_KEYPAD_DOUBLE_VERTICAL_BAR  = 0xCA,
	HID_KEY_KEYPAD_COLON                = 0xCB,
	HID_KEY_KEYPAD_HASH                 = 0xCC,
	HID_KEY_KEYPAD_SPACE                = 0xCD,
	HID_KEY_KEYPAD_AT                   = 0xCE,
	HID_KEY_KEYPAD_EXCLAMATION          = 0xCF,
	HID_KEY_KEYPAD_MEMORY_STORE         = 0xD0,
	HID_KEY_KEYPAD_MEMORY_RECALL        = 0xD1,
	HID_KEY_KEYPAD_MEMORY_CLEAR         = 0xD2,
	HID_KEY_KEYPAD_MEMORY_ADD           = 0xD3,
	HID_KEY_KEYPAD_MEMORY_SUBTRACT      = 0xD4,
	HID_KEY_KEYPAD_MEMORY_MULTIPLY      = 0xD5,
	HID_KEY_KEYPAD_MEMORY_DIVIDE        = 0xD6,
	HID_KEY_KEYPAD_PLUS_MINUS           = 0xD7,
	HID_KEY_KEYPAD_CLEAR                = 0xD8,
	HID_KEY_KEYPAD_CLEAR_ENTRY          = 0xD9,
	HID_KEY_KEYPAD_BINARY               = 0xDA,
	HID_KEY_KEYPAD_OCTAL                = 0xDB,
	HID_KEY_KEYPAD_DECIMAL_2            = 0xDC,
	HID_KEY_KEYPAD_HEXADECIMAL          = 0xDD,
	#Reserved		                    =0xDE-DF,
	HID_KEY_CONTROL_LEFT                = 0xE0,
	HID_KEY_SHIFT_LEFT                  = 0xE1,
	HID_KEY_ALT_LEFT                    = 0xE2,
	HID_KEY_GUI_LEFT                    = 0xE3,
	HID_KEY_CONTROL_RIGHT               = 0xE4,
	HID_KEY_SHIFT_RIGHT                 = 0xE5,
	HID_KEY_ALT_RIGHT                   = 0xE6,
	HID_KEY_GUI_RIGHT                   = 0xE7,
}



enum Pins {
	NC = 0xFFFFFFFF,
	#bank a
	A0  = (0 << 4) + 0x00,
	A1  = (0 << 4) + 0x01,
	A2  = (0 << 4) + 0x02,
	A3  = (0 << 4) + 0x03,
	A4  = (0 << 4) + 0x04,
	A5  = (0 << 4) + 0x05,
	A6  = (0 << 4) + 0x06,
	A7  = (0 << 4) + 0x07,
	A8  = (0 << 4) + 0x08,
	A9  = (0 << 4) + 0x09,
	A10 = (0 << 4) + 0x0A,
	A11 = (0 << 4) + 0x0B,
	A12 = (0 << 4) + 0x0C,
	A13 = (0 << 4) + 0x0D,
	A14 = (0 << 4) + 0x0E,
	A15 = (0 << 4) + 0x0F,
	#bank b
	B0  = (1 << 4) + 0x00,
	B1  = (1 << 4) + 0x01,
	B2  = (1 << 4) + 0x02,
	B3  = (1 << 4) + 0x03,
	B4  = (1 << 4) + 0x04,
	B5  = (1 << 4) + 0x05,
	B6  = (1 << 4) + 0x06,
	B7  = (1 << 4) + 0x07,
	B8  = (1 << 4) + 0x08,
	B9  = (1 << 4) + 0x09,
	B10 = (1 << 4) + 0x0A,
	B11 = (1 << 4) + 0x0B,
	B12 = (1 << 4) + 0x0C,
	B13 = (1 << 4) + 0x0D,
	B14 = (1 << 4) + 0x0E,
	B15 = (1 << 4) + 0x0F,
}
