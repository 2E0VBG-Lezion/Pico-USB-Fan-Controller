#include "pico/stdlib.h"
#include "Fan.h"
#include "Project_Functions.h"
#include "Project_Definitions.h"
#include "hardware/flash.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"

int Project_Functions::VOID_TO_INT(void *ptr)
{
  union void_cast u;
  u.ptr = ptr;
  return u.value;
}

void Project_Functions::TRANSFER_ARRAY(uint s[], uint d[], int n)
{
  for (int i = 0; i < n; i++)
    d[i] = s[i];
}

void *Project_Functions::INT_TO_VOID(int value)
{
  union void_cast u;
  u.value = value;
  return u.ptr;
}
uint Project_Functions::map(uint x, uint in_min, uint in_max, uint out_min, uint out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Project_Functions::INT_TO_BYTES(uint8_t *_buff, int x)
{
  _buff[3] = (x >> 24) & 0xFF;
  _buff[2] = (x >> 16) & 0xFF;
  _buff[1] = (x >> 8) & 0xFF;
  _buff[0] = (x >> 0) & 0xFF;
}

uint Project_Functions::BYTES_TO_INT(uint8_t *_buff)
{
  int num = 0;
  for (int i = 4; i >= 0; i--)
  {
    num <<= 8;
    num |= _buff[i];
  }
  return num;
}

bool Project_Functions::UPDATE_FAN_STATUS(Fan *_fan, uint _FanSpeed) // Update fan function
{
  printf("FanSpeed=%lu\n", _FanSpeed);
  printf("FanSpeed=%lu\n", _fan->GPIO_LED_PIN);

  if (_FanSpeed > 1)
  {
    _fan->LED_STATUS = true;
    _fan->TACHO_FAN_SPEED = _FanSpeed;
    gpio_put(_fan->GPIO_LED_PIN, 1);

  }
  else
  {
    _fan->LED_STATUS = false;
    _fan->TACHO_FAN_SPEED = 0;
    gpio_put(_fan->GPIO_LED_PIN, 0);

  }
  return true;
}

bool Project_Functions::UPDATE_FAN_PROPERTY(Fan *_Fan, int _Property, uint _PropertyValue) // Update fan function
{
  switch (_Property)
  {
  case 0: //  Change pwm value
    _Fan->PWM_VALUE = _PropertyValue;
    break;
  case 1: //  Change temperature sensor
    if (_PropertyValue > 0 && _PropertyValue < 9)
    {
      _Fan->TEMP_SENSOR = _PropertyValue;
    }
    break;
  case 2: // Pwm speed low threasholdC
    _Fan->PWM_SPEED_LOW = _PropertyValue;
    break;
  case 3: // Pwm speed mid threashold
    _Fan->PWM_SPEED_MID = _PropertyValue;
    break;
  case 4: // Pwm speed high threashold
    _Fan->PWM_SPEED_HIGH = _PropertyValue;
    break;
  case 5: //  Change temperature sensor low threashold
    _Fan->TEMP_LOW = _PropertyValue;
    break;
  case 6: //  Change temperature sensor mid threashold
    _Fan->TEMP_MID = _PropertyValue;
    break;
  case 7: //  Change temperature sensor high threashold
    _Fan->TEMP_HIGH = _PropertyValue;
    break;
  case 8: //  Change pwm look at input value
    _Fan->GPIO_PWM_INPUT = _PropertyValue;
    break;
  }
  return true;
}

bool Project_Functions::UPDATE_GENERAL_VARIABLES(int _Property, uint _PropertyValue)
{
  switch (_Property)
  {
  case 0:
    Project_Definitions::GPIO_TEMP_SENSOR_VALUES[7] = _PropertyValue;
    break;
  case 1:
    Project_Definitions::GPIO_TEMP_SENSOR_VALUES[8] = _PropertyValue;
    break;
  case 2:
    Project_Definitions::Mode_Of_Operation = _PropertyValue;
    break;
  case 3:
    Project_Definitions::PWM_READOUT_INTERVAL = _PropertyValue;
    break;
  case 4:
    Project_Definitions::PWM_OUTPUT_INTERVAL = _PropertyValue;
    break;
  case 5:
    Project_Definitions::TACHO_READOUT_INTERVAL = _PropertyValue;
    break;
  }
  return true;
}

uint Project_Functions::RETRIVE_FAN_PROPERTY(Fan _Fan, int _Property)
{
  uint value = 0xFF;
  switch (_Property)
  {
  case 0: // Retrive pwm value
    value = _Fan.PWM_VALUE;
    break;
  case 1: // Retrive temperature sensor
    value = _Fan.TEMP_SENSOR;
    break;
  case 2: // Retrive pwm speed low threasholdC
    value = _Fan.PWM_SPEED_LOW;
    break;
  case 3: // Retrive pwm speed mid threashold
    value = _Fan.PWM_SPEED_MID;
    break;
  case 4: // Retrive pwm speed high threashold
    value = _Fan.PWM_SPEED_HIGH;
    break;
  case 5: //  Retrive temperature sensor low threashold
    value = _Fan.TEMP_LOW;
    break;
  case 6: //  Retrive temperature sensor mid threashold
    value = _Fan.TEMP_MID;
    break;
  case 7: //  Retrive temperature sensor high threashold
    value = _Fan.TEMP_HIGH;
    break;
  case 8: //  Retrive pwm input pin value
    value = _Fan.GPIO_PWM_INPUT;
    break;
  case 9: //  Retrive fan speed value
    value = _Fan.TACHO_FAN_SPEED;
  }
  return value;
}

uint Project_Functions::RETRIVE_GENERAL_VARIABLES(int _Property)
{
  uint value = 0xFF;
  switch (_Property)
  {
  case 0: //  Change pwm value
    value = Project_Definitions::GPIO_TEMP_SENSOR_VALUES[0];
    break;
  case 1: //  Change temperature sensor
    value = Project_Definitions::GPIO_TEMP_SENSOR_VALUES[1];
    break;
  case 2: // Pwm speed low threasholdC
    value = Project_Definitions::GPIO_TEMP_SENSOR_VALUES[2];
    break;
  case 3: // Pwm speed mid threashold
    value = Project_Definitions::GPIO_TEMP_SENSOR_VALUES[3];
    break;
  case 4: // Pwm speed high threashold
    value = Project_Definitions::GPIO_TEMP_SENSOR_VALUES[4];
    break;
  case 5: //  Change temperature sensor low threashold
    value = Project_Definitions::GPIO_TEMP_SENSOR_VALUES[5];
    break;
  case 6: //  Change temperature sensor mid threashold
    value = Project_Definitions::Mode_Of_Operation;
    break;
  case 7: //  Change temperature sensor high threashold
    value = Project_Definitions::PWM_READOUT_INTERVAL;
    break;
  case 8: //  Change pwm look at input value
    value = Project_Definitions::PWM_OUTPUT_INTERVAL;
    break;
  case 9: //  Change pwm look at input value
    value = Project_Definitions::TACHO_READOUT_INTERVAL;
    break;
  }
  return value;
}

int FIND_LATEST_SECTOR()
{
  uint8_t *pointerBuff;
  int IndexVal = 0;
  int FilePageSize = FLASH_PAGE_SIZE;

  // Find latest sector

  for (int page = 0; page < (1u << 12) / FilePageSize; page++)
  {
    int _LookUpAddress = XIP_BASE + (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE) + (page * FilePageSize);
    pointerBuff = (uint8_t *)_LookUpAddress;

    printf("%lu\n", IndexVal);

    if (pointerBuff[0] != 0xFF)
    {
      IndexVal++;
    }
  }
  return IndexVal;
}

void READ_BUFFER_TO_CONFIG(const uint8_t *buf, size_t len)
{
  for (size_t i = 0; i < len; ++i)
  {
    printf("%02x", buf[i]);
    if (i % 16 == 15)
      printf("\n");
    else
      printf(" ");
  }
}

bool Project_Functions::READ_CONFIG_FROM_FLASH()
{
  uint8_t *pointerBuff;
  int page_sector = (FIND_LATEST_SECTOR() - 1);
  int FilePageSize = FLASH_PAGE_SIZE;
  int _FoundAddress = XIP_BASE + PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE + (page_sector * FilePageSize);
  pointerBuff = (uint8_t *)_FoundAddress;
  printf("Reading memory\n");
  printf("%lu\n", FilePageSize);
  READ_BUFFER_TO_CONFIG(pointerBuff, FilePageSize);

  if (pointerBuff[0] == 0x1b)
  {
    int BufferCounter = 4;

    for (int FanIndex = 0; FanIndex < 6; FanIndex++)
    {

      int FanPropertyValue[8];
      for (int FanPropertyIndex = 0; FanPropertyIndex < 8; FanPropertyIndex++)
      {
        uint8_t bytes[4];
        for (int FanPropertyByteIndex = 0; FanPropertyByteIndex < 4; FanPropertyByteIndex++)
        {
          bytes[FanPropertyByteIndex] = pointerBuff[BufferCounter];
          BufferCounter++;
        }
        FanPropertyValue[FanPropertyIndex] = Project_Functions::BYTES_TO_INT(bytes);
      }

      Project_Definitions::Fans[FanIndex].TEMP_SENSOR = FanPropertyValue[0];
      Project_Definitions::Fans[FanIndex].PWM_SPEED_LOW = FanPropertyValue[1];
      Project_Definitions::Fans[FanIndex].PWM_SPEED_MID = FanPropertyValue[2];
      Project_Definitions::Fans[FanIndex].PWM_SPEED_HIGH = FanPropertyValue[3];
      Project_Definitions::Fans[FanIndex].TEMP_LOW = FanPropertyValue[4];
      Project_Definitions::Fans[FanIndex].TEMP_MID = FanPropertyValue[5];
      Project_Definitions::Fans[FanIndex].TEMP_HIGH = FanPropertyValue[6];
      Project_Definitions::Fans[FanIndex].GPIO_PWM_INPUT = FanPropertyValue[7];
    }
    int ControllerPropertyValue[3];
    for (int ControllerPropertyIndex = 0; ControllerPropertyIndex < 3; ControllerPropertyIndex++)
    {
      uint8_t bytes[4];
      for (int ControllerPropertyByteIndex = 0; ControllerPropertyByteIndex < 4; ControllerPropertyByteIndex++)
      {
        bytes[ControllerPropertyByteIndex] = pointerBuff[BufferCounter];
        BufferCounter++;
      }
      ControllerPropertyValue[ControllerPropertyIndex] = Project_Functions::BYTES_TO_INT(bytes);
    }

    Project_Definitions::TACHO_READOUT_INTERVAL = ControllerPropertyValue[0];
    Project_Definitions::PWM_READOUT_INTERVAL = ControllerPropertyValue[1];
    Project_Definitions::PWM_OUTPUT_INTERVAL = ControllerPropertyValue[2];

    return true;
  }
  else
  {
    return false;
  }
}

bool Project_Functions::SAVE_CONFIG_TO_FLASH()
{

  uint8_t *pointerBuff;
  int page_sector = FIND_LATEST_SECTOR();
  int FilePageSize = FLASH_PAGE_SIZE;
  int _region = (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE);
  int pageMark = 1;

  printf("Page sector:\n");
  printf("%u\n", page_sector);
  printf("Starting to do the save.\n");
  uint32_t ints = save_and_disable_interrupts();
  multicore_lockout_start_timeout_us(100);
  printf("Blocking...\n");

  if (page_sector >= 16) // Clear the page, because out of space
  {

    printf("\nErasing target region...\n");
    flash_range_erase(_region, FLASH_SECTOR_SIZE);
    printf("Success.\n");
  }
  else
  {

    printf("Adding page.\n");
    page_sector = page_sector + 1;
  }

  int FanSetSize[8];
  int FileSize = sizeof(pageMark) +
                 (sizeof(FanSetSize) * 6) +
                 sizeof(Project_Definitions::TACHO_READOUT_INTERVAL) +
                 sizeof(Project_Definitions::PWM_READOUT_INTERVAL) +
                 sizeof(Project_Definitions::PWM_OUTPUT_INTERVAL);

  int buf[FileSize / sizeof(int)];

  int BufferCounter = 0;
  buf[BufferCounter] = 0x1b;

  for (int i = 0; i < 6; i++)
  {
    BufferCounter++;
    buf[BufferCounter] = Project_Definitions::Fans[i].TEMP_SENSOR;
    BufferCounter++;
    buf[BufferCounter] = Project_Definitions::Fans[i].PWM_SPEED_LOW;
    BufferCounter++;
    buf[BufferCounter] = Project_Definitions::Fans[i].PWM_SPEED_MID;
    BufferCounter++;
    buf[BufferCounter] = Project_Definitions::Fans[i].PWM_SPEED_HIGH;
    BufferCounter++;
    buf[BufferCounter] = Project_Definitions::Fans[i].TEMP_LOW;
    BufferCounter++;
    buf[BufferCounter] = Project_Definitions::Fans[i].TEMP_MID;
    BufferCounter++;
    buf[BufferCounter] = Project_Definitions::Fans[i].TEMP_HIGH;
    BufferCounter++;
    buf[BufferCounter] = Project_Definitions::Fans[i].GPIO_PWM_INPUT;
  }

  BufferCounter++;
  buf[BufferCounter] = Project_Definitions::TACHO_READOUT_INTERVAL;
  BufferCounter++;
  buf[BufferCounter] = Project_Definitions::PWM_READOUT_INTERVAL;
  BufferCounter++;
  buf[BufferCounter] = Project_Definitions::PWM_OUTPUT_INTERVAL;

  printf("Trying to write data:\n");
  flash_range_program((PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE + (page_sector * FilePageSize)), (uint8_t *)buf, FLASH_PAGE_SIZE);
  printf("Data write complet:\n");

  for (int i = 0; i < FileSize / sizeof(int); i++)
  {
    printf("%02x", buf[i]);
    if (i % 16 == 15)
      printf("\n");
    else
      printf(" ");
  }

  multicore_lockout_end_timeout_us(100);
  restore_interrupts(ints);

  return true;
}