#ifndef PROJECT_DEFINITIONS_H_
#define PROJECT_DEFINITIONS_H_

#include <bitset>
#include "Fan.h"

class Project_Definitions
{
private:
public:
  // Pin setup
  
  static const uint MCU_LED_PIN[2]; // MCU Led's output
  static const uint FAN_LED_PIN[6]; // Led's outputs
  static int GPIO_PWM_OUTPUT[6];    // Pwm outputs

  static const uint GPIO_TACHO_INPUTS[6]; // Tacho inputs
  static const uint GPIO_TACHO_OUTPUT;    // Tacho output

  // TEMP Senors variables
  static const uint GPIO_TEMP_SENSOR_INPUT;              // Sensor pin assigment
  static uint GPIO_TEMP_SENSOR_VALUES[8];                // Temperature values
  static const std::bitset<4> TEMP_SENSOR_PIN_VALUES[6]; // Multiplexer bitarray values
  static const uint TEMP_SENSOR_PIN_LAYOUT[3];           // Multiplexer control pin assigments

  static uint TEMP_HIGH; // High temperature setting
  static uint TEMP_MID;  // Medium temperature setting
  static uint TEMP_LOW;  // Low temperature setting

  // General variables

  static int CORE_1_TIMER_FUNCTION_ENABLED; // is Core 1 timer function enabled?
  static int CORE_1_TIMER_MODE;
  static int CORE_1_INTERVAL;
  
  // PWM measurement variables
  static int GPIO_PWM_INPUT_VALUES[3];  // PWM readout values
  static const uint GPIO_PWM_INPUTS[3]; // PWM Input pin configuration
  static int PWM_READOUT_INTERVAL;      // PWM readout interval

  // PWM Output variables
  static int PWM_OUTPUT_INTERVAL; // PWM output interval

  // PWM Tacho variables
  static int TACHO_READOUT_INTERVAL;


  // General program variables

  static bool Destroy_Measure_Alarms;
  static bool Measure_Fan_Speed;
  static bool StartMeasure;
  static bool on_state;
  static bool old_state;

  static int hallState;
  static int FanPulses;

  static int Mode_Of_Operation;
  // Fan control operation mode:
  // 0 = Driven by pwm inputs from motherboard.
  // 1 = Fully independed mcu control , based on external temp sensors and pc comms.
  // 2 = PC direct drive.

  // Initialisation Functions

  static Fan Fans[6]; // Fans Setup

  static void Initialise(); // Initialises IO
};

#endif
