#ifndef PROJECT_FUNCTIONS_H_
#define PROJECT_FUNCTIONS_H_
#include "Fan.h"

class Project_Functions
{
private:
public:
  union void_cast
  {
    void *ptr;
    int value;
  };

  static int VOID_TO_INT(void *ptr);
  static void *INT_TO_VOID(int value);

  static uint map(uint x, uint in_min, uint in_max, uint out_min, uint out_max);
  static uint BYTES_TO_INT(uint8_t *_buff);
  static void INT_TO_BYTES(uint8_t *_buff, int x);
  static void TRANSFER_ARRAY(uint s[], uint d[], int n);
  static bool UPDATE_FAN_STATUS(Fan *_fan, uint _FanSpeed);
  static bool UPDATE_FAN_PROPERTY(Fan *_Fan, int _Property, uint _PropertyValue);
  static uint RETRIVE_FAN_PROPERTY(Fan _Fan, int _Property);
  static uint RETRIVE_GENERAL_VARIABLES(int _Property);
  static bool UPDATE_GENERAL_VARIABLES(int _Property, uint _PropertyValue);
  static bool SAVE_CONFIG_TO_FLASH();
  static bool READ_CONFIG_FROM_FLASH();
};

#endif
