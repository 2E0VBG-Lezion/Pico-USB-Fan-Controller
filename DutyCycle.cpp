#include "DutyCycle.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "PwmIn.h"
#include "PwmIn.pio.h"
#include "hardware/clocks.h"
#include "Project_Definitions.h"
#include "Project_Functions.h"
#include <string>

// PRIVATE

uint Duty_cycle::Gpio_Pulses_Output[6] = {0, 1, 2, 3, 4, 5};
bool Duty_cycle::Gpio_Pulses_Read[6];

float Duty_cycle::Gpio_Cycle_Output[3] = {0, 1, 2};
bool Duty_cycle::Gpio_Cycle_Read[3];

int32_t Duty_cycle::_alarm_list[6];
bool Duty_cycle::_setup = false;

// PWM Function variables

int Duty_cycle::pwm_measurement_mode = 0;
int Duty_cycle::pwm_current_input_pin_index = 0;
int Duty_cycle::pwm_current_input_pin = 0;
int Duty_cycle::pwm_input_pins_count = 3;
int Duty_cycle::pwm_timeout_period = 4000;

unsigned long Duty_cycle::pwm_duty_cycle_measure_count = 0;
unsigned long Duty_cycle::pwm_duty_cycle_average_max = 2;
unsigned long Duty_cycle::pwm_duty_cycle_average_count = 2;
unsigned long Duty_cycle::pwm_timeout_timer = 0;
unsigned long Duty_cycle::pwm_duty_cycle = 0;

int Duty_cycle::rpm_measurement_mode = 0;
int Duty_cycle::rpm_timeout_period = 10000;
unsigned long Duty_cycle::rpm_timeout_timer = 0;

// PUBLIC

void Duty_cycle::Output_tacho_pwm(int gpio, int value)
{
  uint slice_num = pwm_gpio_to_slice_num(gpio);
  if (_setup == false)
  {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    pwm_set_enabled(slice_num, true);
    pwm_set_wrap(slice_num, value);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, (value / 2));
  }
  else
  {
    pwm_set_wrap(slice_num, value);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, (value / 2));
  }
}

void Duty_cycle::Output_fan_pwm(int gpio, int value)
{
  // Tell GPIO 0 and 1 they are allocated to the PWM

  gpio_set_function(gpio, GPIO_FUNC_PWM);

  // Find out which PWM slice is connected to GPIO 0 (it's slice 0)
  uint slice_num = pwm_gpio_to_slice_num(gpio);
  pwm_config cfg = pwm_get_default_config();
  pwm_init(slice_num, &cfg, false);

  pwm_set_clkdiv(slice_num, 1);
  // Set period of 4 cycles (0 to 3 inclusive)
  pwm_set_wrap(slice_num, 5000);
  // Set channel A output high for one cycle before dropping
  pwm_set_chan_level(slice_num, PWM_CHAN_A, value);
  // Set initial B output high for three cycles before dropping
  // Set the PWM running
  pwm_set_enabled(slice_num, true);
  // Note we could also use pwm_set_gpio_level(gpio, x) which looks up the
  // correct slice and channel for a given GPIO.
}

// Measures incoming PWM signal and calculates duty cycle
// Takes no arguments
void Duty_cycle::Measure_fan_pwm()
{
  switch (pwm_measurement_mode)
  {
  case 0:
    // Set IRQ to active pin
    {
      pwm_current_input_pin = Project_Definitions::GPIO_PWM_INPUTS[pwm_current_input_pin_index];
      pwm_timeout_timer = time_us_32();
      pwm_measurement_mode = 1;
    }
    break;

  case 1:
    // Measure pwm pulses
    {
      if ((pwm_timeout_timer + pwm_timeout_period) > time_us_32())
      {
        if (gpio_get(pwm_current_input_pin) == 0)
        {
          pwm_duty_cycle_measure_count++;
        }
      }
      else
      {
        if (pwm_duty_cycle_average_count < pwm_duty_cycle_average_max)
        {
          pwm_duty_cycle = pwm_duty_cycle + pwm_duty_cycle_measure_count;
          pwm_duty_cycle_measure_count = 0;
          pwm_duty_cycle_average_count++;
        }
        else
        {
          // Measure done or timeout
          if (pwm_current_input_pin_index < pwm_input_pins_count)
          {
            pwm_duty_cycle = pwm_duty_cycle / pwm_duty_cycle_average_max;
            Project_Definitions::GPIO_PWM_INPUT_VALUES[pwm_current_input_pin_index] = pwm_duty_cycle*1.98;

            // Increment pin, and return to mode 0

            // reset variables

            pwm_duty_cycle_average_count = 0;
            pwm_duty_cycle_measure_count = 0;
            pwm_duty_cycle = 0;
            pwm_measurement_mode = 0;

            // increment pin
            pwm_current_input_pin_index++;
          }
          else
          {
            // All pins measured, proceed to exit
            // reset variables first

            pwm_duty_cycle_average_count = 0;
            pwm_duty_cycle_measure_count = 0;
            pwm_duty_cycle = 0;
            pwm_measurement_mode = 0;
            pwm_current_input_pin_index = 0;

            // proceed to exit

            pwm_measurement_mode = 2;
          }
        }
      }
    }
    break;

  case 2:
    // Exit sequence, turn off all the interrupts
    {
      // Reset mode to 0
      pwm_measurement_mode = 0;
      // Disable the function
      Project_Definitions::CORE_1_TIMER_FUNCTION_ENABLED = false;
    }
    break;
  }
}

void Duty_cycle::Measure_fan_rpm()
{
  switch (rpm_measurement_mode)
  {
  case 0:
  {
    PwmIn::PioPwm0.Enable_IRQ(true);
    PwmIn::PioPwm1.Enable_IRQ(true);
    rpm_timeout_timer = time_us_32();
    rpm_measurement_mode = 1;
  }
  break;
  case 1:
  {
    if ((rpm_timeout_timer + rpm_timeout_period) > time_us_32())
    {
      Project_Functions::UPDATE_FAN_STATUS(&Project_Definitions::Fans[0], (uint)PwmIn::PioPwm0.read_P(0));
      Project_Functions::UPDATE_FAN_STATUS(&Project_Definitions::Fans[1], (uint)PwmIn::PioPwm0.read_P(1));
      Project_Functions::UPDATE_FAN_STATUS(&Project_Definitions::Fans[2], (uint)PwmIn::PioPwm0.read_P(2));
      Project_Functions::UPDATE_FAN_STATUS(&Project_Definitions::Fans[3], (uint)PwmIn::PioPwm0.read_P(3));
      Project_Functions::UPDATE_FAN_STATUS(&Project_Definitions::Fans[4], (uint)PwmIn::PioPwm1.read_P(0));
      Project_Functions::UPDATE_FAN_STATUS(&Project_Definitions::Fans[5], (uint)PwmIn::PioPwm1.read_P(1));
    }
    else
    {
      rpm_measurement_mode = 2;
    }
  }
  break;
  case 2:
  {
    PwmIn::PioPwm0.Enable_IRQ(false);
    PwmIn::PioPwm1.Enable_IRQ(false);
    rpm_timeout_timer = 0;
    rpm_measurement_mode = 0;
    Project_Definitions::CORE_1_TIMER_FUNCTION_ENABLED = false;
  }
  break;
  }
}