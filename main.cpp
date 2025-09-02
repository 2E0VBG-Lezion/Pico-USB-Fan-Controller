#include <stdio.h>
#include <string.h>
#include "Project_Definitions.h"
#include "Project_Functions.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"

#include "DutyCycle.h"
#include "hardware/irq.h"
#include "hardware/adc.h"

#include "usb_functions.h"

#include <bitset>

#define NUM(a) (sizeof(a) / sizeof(a))

// Fans struct data entry

int FanIndex = 0;
Duty_cycle DutyCycle;

// General variables

uint32_t previousMillis = 0;
long CurrentFanSpeed = 0;
volatile bool timer_fired = false;

// Timers
struct repeating_timer CORE_0_CONTROL_TIMER;
struct repeating_timer CORE_1_CONTROL_TIMER;

alarm_pool_t *Main_Alarm_Pool;

// Helping functions

void Output_Tacho()
{
  int count = 0;
  int avarageValue = 0;
  for (int i = 0; i < 5; i++)
  {
    if (Project_Definitions::Fans[i].TACHO_FAN_SPEED > 0)
    {
      count = count + 1;
      avarageValue = avarageValue + Project_Definitions::Fans[i].TACHO_FAN_SPEED;
    }
  }
  // DutyCycle.Output_tacho_pwm(Project_Definitions::GPIO_TACHO_OUTPUT, (avarageValue / count));
}

void Output_Fan()
{
  for (int i = 0; i < 6; i++)
  {
    Duty_cycle::Output_fan_pwm(Project_Definitions::Fans[i].GPIO_PWM_OUTPUT_PIN, Project_Definitions::Fans[i].PWM_VALUE);
  }
}

bool CORE_0_TIMER_CALLBACK(struct repeating_timer *t)
{
  switch (Project_Definitions::Mode_Of_Operation)
  {

  case 0: // 0 = Driven by pwm inputs from motherboard.

    for (int a = 0; a < 6; a++)
    {
      Project_Definitions::Fans[a].PWM_VALUE = Project_Definitions::GPIO_PWM_INPUT_VALUES[Project_Definitions::Fans[a].GPIO_PWM_INPUT];
    }
    break;

  case 1: // 1 = Fully independed mcu control , based on external temp sensors and pc comms.

    for (int a = 0; a < 6; a++)
    {
      uint Temp_Value = Project_Definitions::GPIO_TEMP_SENSOR_VALUES[Project_Definitions::Fans[a].TEMP_SENSOR];

      if (Temp_Value > Project_Definitions::TEMP_HIGH)
      {
        Project_Definitions::Fans[a].PWM_SPEED_HIGH;
      }

      else if (Temp_Value < Project_Definitions::TEMP_HIGH && Temp_Value > Project_Definitions::TEMP_MID)
      {
        Project_Definitions::Fans[a].PWM_VALUE = Project_Functions::map(Temp_Value, Project_Definitions::TEMP_MID, Project_Definitions::TEMP_HIGH, Project_Definitions::Fans[a].PWM_SPEED_MID, Project_Definitions::Fans[a].PWM_SPEED_HIGH);
      }

      else if (Temp_Value < Project_Definitions::TEMP_MID && Temp_Value > Project_Definitions::TEMP_LOW)
      {
        Project_Definitions::Fans[a].PWM_VALUE = Project_Functions::map(Temp_Value, Project_Definitions::TEMP_LOW, Project_Definitions::TEMP_MID, Project_Definitions::Fans[a].PWM_SPEED_LOW, Project_Definitions::Fans[a].PWM_SPEED_MID);
      }

      else if (Temp_Value < Project_Definitions::TEMP_LOW)
      {
        Project_Definitions::Fans[a].PWM_VALUE = Project_Definitions::Fans[a].PWM_SPEED_LOW;
      }
      
    }
    break;

  case 2: // 2 = PC direct drive.

    break;
  }

  // Output_Tacho();
  Output_Fan();

  return true;
}

bool CORE_1_TIMER_CALLBACK(struct repeating_timer *t)
{
  Project_Definitions::CORE_1_TIMER_FUNCTION_ENABLED = false;
  //printf("Core 1 timer callback\n");

  if (Project_Definitions::CORE_1_TIMER_MODE > 1)
  {
    Project_Definitions::CORE_1_TIMER_MODE = 0;
  }
  else
  {
    Project_Definitions::CORE_1_TIMER_MODE++;
  }

  Project_Definitions::CORE_1_TIMER_FUNCTION_ENABLED = true;
  return true;
}

bool CORE_1_TIMER_FUNCTION(int _TIMER_MODE)
{
  switch (_TIMER_MODE)
  {
  case 0:
    Duty_cycle::Measure_fan_pwm();
    break;
  case 1:
    Duty_cycle::Measure_fan_rpm();
    break;
  }
  return true;
}

void MAIN_CORE_1()
{
  multicore_lockout_victim_init();

  // Initialize Tacho Counting

  add_repeating_timer_ms(Project_Definitions::CORE_1_INTERVAL, CORE_1_TIMER_CALLBACK, NULL, &CORE_1_CONTROL_TIMER);

  while (1)
  {

    tight_loop_contents();
    if (Project_Definitions::CORE_1_TIMER_FUNCTION_ENABLED == true)
    {
      CORE_1_TIMER_FUNCTION(Project_Definitions::CORE_1_TIMER_MODE);
    }
  }
}

int main()
{
  stdio_init_all();
  uart_init(uart0, 115200);



  // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
  gpio_set_function(0, GPIO_FUNC_UART);
  gpio_set_function(1, GPIO_FUNC_UART);

  Project_Definitions::Initialise();
  Project_Functions::READ_CONFIG_FROM_FLASH();

  multicore_launch_core1(MAIN_CORE_1);

  add_repeating_timer_ms(Project_Definitions::PWM_OUTPUT_INTERVAL, CORE_0_TIMER_CALLBACK, NULL, &CORE_0_CONTROL_TIMER);

  usb_functions::USB_Initialize_Device();

  while (1)
  {
    usb_functions::USB_Task_Device();
  }
  return 0;
}
