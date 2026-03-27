#pragma once
#include <Arduino.h>
#include "pin_declaration.h"

struct PinMapping {
  PinName pin_name = NC;
  //Pin* pin = nullptr;
  u_int8_t input_type = 0;
  u_int16_t input_id = 0;

  int16_t value = 0;
  int16_t previous_value = 0;

  bool invert = false;

  u_int16_t max_report_value = 255;
  u_int16_t activation_value = 50; 
  //int range = ADC_MAX_VALUE / 2; //might still be needed, haven't gotten to that step yet.

  float scale = (float) max_report_value / (float) ADC_MAX_VALUE + 0.02;
  u_int8_t deadzone = (float) 255 * 0.04;

  u_int16_t change_amount_before_update = (float) max_report_value * 0.01;

  bool quick_release = false;
  int counter_strafe_help_time_ms = 0;

  PinMapping(
      PinName pin = NC,
      int type = 0,
      u_int16_t id = 0,
      bool isInverted = false,
      bool quickRelease = false,
      int counterStrafeHelpTime = 0
  )
    : pin_name(pin), 
    input_type(type), 
    input_id(id),
    invert(isInverted), 
    //these are unimplemented
    quick_release(quickRelease), 
    counter_strafe_help_time_ms(counterStrafeHelpTime),
    value(0), 
    previous_value(0) 
    {
      //TODO add a check to make sure the selected pin is present in the pin map
      //if (pin_name != NC) this->pin = &pin_map[pin_name];
    }
  

  void update_value();

  int get_value_digital() const;
  int get_value_analog() const;
  int read_value() const;

  int get_value() const;

  bool is_pressed() const;
  bool is_released() const;
  bool is_pressed(int test_value) const;
  bool is_released(int test_value) const;

  bool is_just_pressed() const;
  bool is_just_released() const;
  bool is_just_changed() const;

  #define MAPPING_BYTE_SIZE 17U //pin_name, input_type, input_id, input_id, invert, max_report_value, max_report_value, activation_value, activation_value, scale_bits, scale_bits, scale_bits, scale_bits, deadzone, deadzone, change_amount_before_update, quick_release, counter_strafe_help_time_ms
  void to_bytes(u_int8_t *&return_buffer) const {
    *return_buffer++ = pin_name;
    *return_buffer++ = input_type;
    *return_buffer++ = input_id >> 8 & 0xFF;
    *return_buffer++ = input_id & 0xFF;
    *return_buffer++ = invert;
    
    *return_buffer++ = max_report_value >> 8 & 0xFF;
    *return_buffer++ = max_report_value & 0xFF;
    *return_buffer++ = activation_value >> 8 & 0xFF;
    *return_buffer++ = activation_value & 0xFF;
    //TODO: this is a float, there might be a better way to store this data.
    u_int32_t scale_bytes = 0;
    memcpy(&scale_bytes, &scale, 4);
    *return_buffer++ = scale_bytes >> 24 & 0xFF;
    *return_buffer++ = scale_bytes >> 16 & 0xFF;
    *return_buffer++ = scale_bytes >> 8 & 0xFF;
    *return_buffer++ = scale_bytes & 0xFF;

    //*return_buffer++ = deadzone >> 8 & 0xFF;
    *return_buffer++ = deadzone;// & 0xFF;
    
    *return_buffer++ = change_amount_before_update;
    *return_buffer++ = quick_release;
    *return_buffer++ = counter_strafe_help_time_ms;
  }

  //TODO
  //static func from_bytes(bytes : PackedByteArray) -> InputMapping:
	//if bytes.size() != BYTE_SIZE:
	//	push_error("Passed byte array does not match the byte size of PinDeclaration.")
	//
	//var _pin_name : int = bytes[0]
	//var _input_type : int = bytes[1]
	//var _input_id : int = bytes[2] + (bytes[3] << 8)
	//var _invert : bool = bytes[4]
	//var _max_report_value : int = (bytes[5] << 8) + bytes[6]
	//var _activation_value : int = (bytes[7] << 8) + bytes[8]
	//
	//var scale_bytes := bytes.slice(9, 13)
	//scale_bytes.reverse()
	//var _scale : float = scale_bytes.decode_float(0) #10, 11, 12 #TODO: verify this is correct
	//var _deadzone : int = bytes[13]
	//var _change_amount_before_update : int = bytes[14]
	//var _quick_release : bool = bytes[15]
	//var _counter_strafe_help_time_ms : int = bytes[16]
	//
	//return InputMapping.new(_pin_name, _input_type, _input_id, _invert, _max_report_value,\
	//_activation_value, _scale, _deadzone, _change_amount_before_update, _quick_release, \
	//_counter_strafe_help_time_ms)
  //
  //static func from_byte_array(bytes : PackedByteArray) -> Array[InputMapping]:
	//var array : Array[InputMapping] = []
	//
	//for index in range(0, bytes.size(), BYTE_SIZE):
	//	array.append(from_bytes(bytes.slice(index, index + BYTE_SIZE)))
	//
	//return array
};