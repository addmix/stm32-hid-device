#pragma once
#include <Arduino.h>
#include "pin_declaration.h"

#define MAPPING_BYTE_SIZE 17U //pin_name, input_type, input_id, input_id, invert, max_report_value, max_report_value, activation_value, activation_value, scale_bits, scale_bits, scale_bits, scale_bits, deadzone, deadzone, change_amount_before_update, quick_release, counter_strafe_help_time_ms
struct PinMapping {
  static const u_int8_t BYTE_SIZE = MAPPING_BYTE_SIZE;
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
  u_int8_t counter_strafe_help_time_ms = 0;

  PinMapping(
      PinName pin = NC,
      u_int8_t type = 0,
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
  

      PinMapping(
      //TODO: half of these are unimplemented
      PinName pin,
      u_int8_t type,
      u_int16_t id,
      bool isInverted,
      u_int16_t max_report_value,
      u_int16_t activation_value,
      float scale,
      u_int8_t deadzone,
      u_int16_t change_amount_before_update,
      bool quickRelease,
      int counterStrafeHelpTime
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

  
  void print() {
    Serial.println(
    "pin number=" + (String) pin_name + 
    " input device=" + (String) input_type +
    " input id=" + (String) input_id +
    " inverted=" + (String) invert +
    " max report value=" + (String) max_report_value +
    " activation value=" + (String) activation_value +
    " scale=" + (String) scale +
    " deadzone=" + (String) deadzone +
    " change amount before update=" + (String) change_amount_before_update +
    " quick release=" + (String) quick_release +
    " counter strafe help time ms=" + (String) counter_strafe_help_time_ms
    );
  }

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


 
  static PinMapping from_bytes(u_int8_t *&read_buffer) {//static func from_bytes(bytes : PackedByteArray) -> InputMapping:
    //if bytes.size() != BYTE_SIZE:
    //	push_error("Passed byte array does not match the byte size of PinDeclaration.")
    //
    PinName pin_name = (PinName) *read_buffer++;//var _pin_name : int = bytes[0]
    u_int8_t input_type = *read_buffer++;//var _input_type : int = bytes[1]
    u_int16_t input_id = ((u_int16_t) *read_buffer++ << 8 ) + (u_int16_t) *read_buffer++;//var _input_id : int = bytes[2] + (bytes[3] << 8)
    bool invert = *read_buffer++;//var _invert : bool = bytes[4]
    u_int16_t max_report_value = ((u_int16_t) *read_buffer++ << 8 ) + (u_int16_t) *read_buffer++; //var _max_report_value : int = (bytes[5] << 8) + bytes[6]
    u_int16_t activation_value = ((u_int16_t) *read_buffer++ << 8 ) + (u_int16_t) *read_buffer++; //var _activation_value : int = (bytes[7] << 8) + bytes[8]
    u_int32_t scale_bytes = ((u_int16_t) *read_buffer++ << 24 ) + ((u_int16_t) *read_buffer++ << 16 ) + ((u_int16_t) *read_buffer++ << 8 ) + (u_int16_t) *read_buffer++;
    float scale = 0x00000000;
    memcpy(&scale, &scale_bytes, 4); //TODO check that this actually works
    //var scale_bytes := bytes.slice(9, 13)
    //scale_bytes.reverse()
    //var _scale : float = scale_bytes.decode_float(0) #10, 11, 12 #TODO: verify this is correct

    u_int8_t deadzone = *read_buffer++;//var _deadzone : int = bytes[13]
    u_int16_t change_amount_before_update = /*((u_int16_t) *read_buffer++ << 8 ) +*/ (u_int16_t) *read_buffer++;//var _change_amount_before_update : int = bytes[14]
    bool quick_release = *read_buffer++;//var _quick_release : bool = bytes[15]
    u_int8_t counter_strafe_help_time_ms = *read_buffer++;//var _counter_strafe_help_time_ms : int = bytes[16]
    
    
    return PinMapping(pin_name, input_type, input_id, invert, max_report_value,\
    activation_value, scale, deadzone, change_amount_before_update, quick_release, \
    counter_strafe_help_time_ms);
  }

  static void from_byte_array(u_int8_t *&read_buffer, PinMapping *&return_buffer, u_int16_t items_to_parse) {//static func from_byte_array(bytes : PackedByteArray) -> Array[InputMapping]:
    for (size_t i = 0; i < items_to_parse; ++i) {
      *return_buffer++ = from_bytes(read_buffer);
    }
  }
};