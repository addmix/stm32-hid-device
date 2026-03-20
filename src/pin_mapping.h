#pragma once
#include <Arduino.h>
#include "pin_declaration.h"
//#define ADC_MAX_VALUE 1024

struct PinMapping {
  PinName pin_name = NC;
  Pin* pin = nullptr;
  int input_type = 0;
  u_int16_t input_id = 0;

  int value = 0;
  int previous_value = 0;

  //bool analog = false;
  bool invert = false;

  int max_report_value = 256;
  int activation_value = 50;
  //int center = ADC_MAX_VALUE / 2; 
  //int range = ADC_MAX_VALUE / 2;

  float scale = (float) max_report_value / (float) ADC_MAX_VALUE + 0.05;
  float deadzone_percent = 0.05;

  int change_amount_before_update = ((float) ADC_MAX_VALUE * 0.005);

  bool quick_release = false;
  int counter_strafe_help_time_ms = 0;

  PinMapping(
      PinName pin = NC,
      int type = 0,
      u_int16_t id = 0,
      //bool isAnalog = false,
      bool isInverted = false,
      bool quickRelease = false,
      int counterStrafeHelpTime = 0
  )
    : pin_name(pin), 
    input_type(type), 
    input_id(id),
    //analog(isAnalog), 
    invert(isInverted), 
    //these are unimplemented
    quick_release(quickRelease), 
    counter_strafe_help_time_ms(counterStrafeHelpTime),
    value(0), 
    previous_value(0) 
    {
      //TODO add a check to make sure the selected pin is present in the pin map
      if (pin_name != NC) this->pin = &pin_map[pin_name];
    }
  

  void update_value();

  const int get_value_digital();
  const int get_value_analog();
  const int read_value();

  const int get_value();

  const bool is_pressed();
  const bool is_released();
  const bool is_pressed(int test_value);
  const bool is_released(int test_value);

  const bool is_just_pressed();
  const bool is_just_released();
  const bool is_just_changed();
};