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

  #define MAPPING_BYTE_SIZE 18U //pin_name, input_type, input_id, input_id, invert, max_report_value, max_report_value, activation_value, activation_value, scale_bits, scale_bits, scale_bits, scale_bits, deadzone, deadzone, change_amount_before_update, quick_release, counter_strafe_help_time_ms
  void mapping_to_bytes(u_int8_t *&return_buffer) const {
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
    u_int32_t scale_bits = 0;
    memcpy(&scale_bits, &scale, 4);
    *return_buffer++ = scale_bits >> 24 & 0xFF;
    *return_buffer++ = scale_bits >> 16 & 0xFF;
    *return_buffer++ = scale_bits >> 8 & 0xFF;
    *return_buffer++ = scale_bits & 0xFF;

    //*return_buffer++ = deadzone >> 8 & 0xFF;
    *return_buffer++ = deadzone;// & 0xFF;
    
    *return_buffer++ = change_amount_before_update;
    *return_buffer++ = quick_release;
    *return_buffer++ = counter_strafe_help_time_ms;
  }
};