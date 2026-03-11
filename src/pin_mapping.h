
#pragma once
#include <Arduino.h>

#define DEBOUNCE_TICKS_MS 15
#define ADC_MAX_VALUE 1024

struct PinMapping {
  PinName pin_name = NC;
  int input_type = 0;
  uint16_t input_id = 0;

  int value = 0;
  int previous_value = 0;
  uint last_change_time = 0;

  bool analog = false;
  bool invert = false;

  int max_report_value = 256;
  int activation_value = 30;
  int center = ADC_MAX_VALUE / 2;
  int range = center;

  float scale = (float) max_report_value / (float) ADC_MAX_VALUE + 0.05;
  float deadzone_percent = 0.05;

  int change_amount_before_update = ((float) ADC_MAX_VALUE * 0.005);

  bool quick_release = false;
  int counter_strafe_help_time_ms = 0;

  PinMapping(
      PinName pin = NC,
      int type = 0,
      uint16_t id = 0,
      bool isAnalog = false,
      bool isInverted = false,
      bool quickRelease = false,
      int counterStrafeHelpTime = 0
  );

  void update_value();

  int get_value_digital();
  int get_value_analog();
  int read_value();

  const int get_value();

  const bool is_pressed(int test_value = -1);
  const bool is_released(int test_value = -1);

  const bool is_just_pressed();
  const bool is_just_released();
  const bool is_just_changed();

  const bool is_bounce();
};