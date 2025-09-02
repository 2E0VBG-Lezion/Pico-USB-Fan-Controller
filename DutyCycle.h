#ifndef DUTYCYCYLE_H
#define DUTYCYCYLE_H
#include <stdio.h>
#include "pico/stdlib.h"

class Duty_cycle
{
private:
  static bool _count_start;
  static bool _setup;
  static int32_t _alarm_list[6];

  static bool Gpio_Pulses_Read[6];
  static uint Gpio_Pulses_Output[6];

  static float Gpio_Cycle_Output[3];
  static bool Gpio_Cycle_Read[3];

  // PWM Function variables

  static int pwm_measurement_mode;
  static int pwm_current_input_pin_index;
  static int pwm_current_input_pin;
  static int pwm_input_pins_count;
  static int pwm_timeout_period;
  
  static unsigned long pwm_duty_cycle_average_max;
  static unsigned long pwm_duty_cycle_average_count;
  static unsigned long pwm_duty_cycle_measure_count;
  static unsigned long pwm_timeout_timer;
  static unsigned long pwm_duty_cycle;


  static void pwm_interrupt_event(uint gpio, uint32_t events);

  // RPM Function variables

  static int rpm_measurement_mode;
  static int rpm_timeout_period;
  static unsigned long rpm_timeout_timer;

public:
  static void Output_tacho_pwm(int gpio, int value);
  static void Output_fan_pwm(int gpio, int value);

  // Measures incoming PWM signal and calculates duty cycle
  // Takes no arguments
  static void Measure_fan_pwm();
  static void Measure_fan_rpm();
};

#endif // DUTYCYCYLE_H