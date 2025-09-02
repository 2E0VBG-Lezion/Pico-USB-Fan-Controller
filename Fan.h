#ifndef Fan_H
#define Fan_H

class Fan
{
private:
public:

  int GPIO_LED_PIN;
  int GPIO_PWM_OUTPUT_PIN;
  int GPIO_PWM_INPUT; // GPIO PWM Input pin index
  int GPIO_FAN_TACHO;
  int PWM_SPEED_LOW;
  int PWM_SPEED_MID;
  int PWM_SPEED_HIGH;
  int TEMP_HIGH; // High temperature setting
  int TEMP_MID;  // Medium temperature setting
  int TEMP_LOW;  // Low temperature setting
  int TEMP_SENSOR;
  int PWM_VALUE;
  long TACHO_FAN_SPEED;
  bool LED_STATUS;
};

#endif
