/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Nathan Conrad
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "tusb.h"
#include "bsp/board.h"
#include "usb_comms.h"
#include "pico/stdlib.h"
#include "Project_Functions.h"
#include "Project_Definitions.h"

static bool usb_connected = true;

void string2hexString(char input[], uint8_t *output)
{
  int length = 5;
  for (int i = 0; i < length; i++)
  {
    uint8_t val = (uint8_t)input[i];
    output[i] = val;
  }
}

void LedGPO(uint PIN, int Arg)
{
  gpio_init(PIN);
  gpio_set_dir(PIN, GPIO_OUT);
  gpio_put(PIN, Arg);
}

void change(char *array, int length, char value[])
{
  printf("array address inside function: %p\n", array);
  int i;
  for (i = 0; i < length; i++)
    array[i] = value[i];
}

u_int8_t usb_comms_send_command(uint8_t data[], uint32_t count)
{
  bool boolean = false;

  if (usb_connected)
  {
    tud_vendor_write(data, count);
    boolean = true;
  }

  return boolean;
}

u_int8_t usb_comms_receive_command(uint8_t buf[], uint32_t count)
{
  uint8_t intData[4];
  int intBuff = 0;
  bool isUpdated;

  printf("%u\n", buf[0]);

  switch (buf[0])
  {

  case 0x0: // Update fan variables

    isUpdated = Project_Functions::UPDATE_FAN_PROPERTY(&Project_Definitions::Fans[buf[1]], buf[2], buf[3]);
    Project_Functions::INT_TO_BYTES(intData, isUpdated);
    usb_comms_send_command(intData, 4);
    break;

  case 0x1: // Update general variables

    isUpdated = Project_Functions::UPDATE_GENERAL_VARIABLES(buf[1], buf[2]);
    Project_Functions::INT_TO_BYTES(intData, isUpdated);
    usb_comms_send_command(intData, 4);

    break;

  case 0x2: // Retrive general variables

    intBuff = Project_Functions::RETRIVE_GENERAL_VARIABLES(buf[1]);
    Project_Functions::INT_TO_BYTES(intData, intBuff);
    usb_comms_send_command(intData, 4);

    break;

  case 0x3: // Retrive fan variables

    intBuff = Project_Functions::RETRIVE_FAN_PROPERTY(Project_Definitions::Fans[buf[1]], buf[2]);
    Project_Functions::INT_TO_BYTES(intData, intBuff);
    usb_comms_send_command(intData, 4);

    break;

  case 0x4: // Save configuration

    intBuff = Project_Functions::SAVE_CONFIG_TO_FLASH();
    Project_Functions::INT_TO_BYTES(intData, intBuff);
    usb_comms_send_command(intData, 4);
    break;

  case 0x5: // Load configuration to memory

    intBuff = Project_Functions::READ_CONFIG_FROM_FLASH();
    Project_Functions::INT_TO_BYTES(intData, intBuff);
    usb_comms_send_command(intData, 4);
    break;

  }

  return 0;
}

void usb_comms_task(void)
{
  if (usb_connected)
  {

    if (tud_vendor_available())
    {
      uint8_t buf[64];
      uint32_t count = tud_vendor_read(buf, sizeof(buf));
      printf("Printing buffer\n");
      printf("%u\n", buf[0]);
      printf("%u\n", buf[1]);
      usb_comms_receive_command(buf, count);
    }
  }
}
