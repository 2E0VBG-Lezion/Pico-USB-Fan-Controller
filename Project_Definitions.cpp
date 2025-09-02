#include "pico/stdlib.h"
#include <bitset>
#include "PwmIn.h"
#include "hardware/adc.h"
#include "Project_Definitions.h"
#include "Project_Functions.h"

const uint Project_Definitions::MCU_LED_PIN[2] = {29, 25};                  // MCU Led's output
const uint Project_Definitions::FAN_LED_PIN[6] = {14, 16, 18, 19, 27, 28};  // Led's outputs
int Project_Definitions::GPIO_PWM_OUTPUT[6] = {2, 4, 6, 8, 10, 12};         // Pwm outputs
const uint Project_Definitions::GPIO_TACHO_INPUTS[6] = {1, 3, 5, 7, 9, 11}; // Tacho inputs
const uint Project_Definitions::GPIO_TACHO_OUTPUT = 0;

// General variables

int Project_Definitions::CORE_1_TIMER_FUNCTION_ENABLED = false; // is Core 1 timer function enabled?
int Project_Definitions::CORE_1_TIMER_MODE = 0;                 // Core 1 timer mode
int Project_Definitions::CORE_1_INTERVAL = 1000;
// PWM measurement variables

int Project_Definitions::GPIO_PWM_INPUT_VALUES[3] = {0, 1, 2};     // PWM readout values
const uint Project_Definitions::GPIO_PWM_INPUTS[3] = {13, 15, 17}; // PWM Input pin configuration
int Project_Definitions::PWM_READOUT_INTERVAL = 1000;              // PWM readout interval

// PWM Output variables
int Project_Definitions::PWM_OUTPUT_INTERVAL = 1000; // PWM output interval

// PWM Tacho variables
int Project_Definitions::TACHO_READOUT_INTERVAL = 1000; // Tacho readout interval
bool Project_Definitions::Destroy_Measure_Alarms = false;
bool Project_Definitions::Measure_Fan_Speed = false;
bool Project_Definitions::StartMeasure = true;
bool Project_Definitions::on_state = false;
bool Project_Definitions::old_state = true;
int Project_Definitions::hallState = 0;
int Project_Definitions::FanPulses = 0;

int Project_Definitions::Mode_Of_Operation = 0;
// Fan control operation mode:
// 0 = Driven by pwm inputs from motherboard.
// 1 = Fully independed mcu control , based on external temp sensors and pc comms.
// 2 = PC direct drive.

// TEMP Senors variables

uint Project_Definitions::TEMP_HIGH = 80; // High temperature setting, Default value = 80
uint Project_Definitions::TEMP_MID = 50;  // Medium temperature setting, Default value = 80
uint Project_Definitions::TEMP_LOW = 30;  // Low temperature setting, Default value = 80

#define SENSOR_Y0 0x0 // 0000
#define SENSOR_Y1 0x1 // 0001
#define SENSOR_Y2 0x2 // 0010
#define SENSOR_Y3 0x3 // 0011
#define SENSOR_Y4 0x4 // 0100
#define SENSOR_Y5 0x5 // 0101

const std::bitset<4> Project_Definitions::TEMP_SENSOR_PIN_VALUES[6] = {SENSOR_Y0, SENSOR_Y1, SENSOR_Y2, SENSOR_Y3, SENSOR_Y4, SENSOR_Y5};
uint Project_Definitions::GPIO_TEMP_SENSOR_VALUES[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Temperature values 7 and 8 value reserved for PC Comms

const uint Project_Definitions::TEMP_SENSOR_PIN_LAYOUT[3] = {22, 23, 24};
const uint Project_Definitions::GPIO_TEMP_SENSOR_INPUT = 26;
Fan Project_Definitions::Fans[6];

// Void

void Project_Definitions::Initialise()
{
    adc_init();
    adc_gpio_init(GPIO_TEMP_SENSOR_INPUT);
    adc_select_input(0);

    // Set PIO for PWM in {
    uint pin_list_One[4] = {1, 3, 5, 7};
    uint pin_list_Two[2] = {9, 11};

    // set PIO 0 PWMin

    PwmIn SetPio0Pwm;
    SetPio0Pwm.num_of_pins = 4;
    SetPio0Pwm.PIOinstance = 0;
    Project_Functions::TRANSFER_ARRAY(pin_list_One, SetPio0Pwm.pin_list, 4);
    PwmIn::PioPwm0 = SetPio0Pwm;
    PwmIn::PioPwm0.ConstructPwmIn();

    // set PIO 1 PWMin

    PwmIn SetPio1Pwm;
    SetPio1Pwm.num_of_pins = 2;
    SetPio1Pwm.PIOinstance = 1;
    Project_Functions::TRANSFER_ARRAY(pin_list_Two, SetPio1Pwm.pin_list, 2);
    PwmIn::PioPwm1 = SetPio1Pwm;
    PwmIn::PioPwm1.ConstructPwmIn();
    //}

    // LED
    for (int a = 0; a < 2; a++)
    {
        gpio_init(MCU_LED_PIN[a]);
        gpio_set_dir(MCU_LED_PIN[a], GPIO_OUT);
    }

    for (int a = 0; a < 6; a++)
    {
        gpio_init(FAN_LED_PIN[a]);
        gpio_set_dir(FAN_LED_PIN[a], GPIO_OUT);
    }

    for (int Sensor_Pin = 0; Sensor_Pin < 3; Sensor_Pin++)
    {
        gpio_init(TEMP_SENSOR_PIN_LAYOUT[Sensor_Pin]);
        gpio_set_dir(TEMP_SENSOR_PIN_LAYOUT[Sensor_Pin], GPIO_OUT);
    }

    for (int pwm_pin = 0; pwm_pin < 3; pwm_pin++)
    {
        gpio_init(GPIO_PWM_INPUTS[pwm_pin]);
        gpio_set_dir(GPIO_PWM_INPUTS[pwm_pin], GPIO_IN);
    }

    gpio_put(MCU_LED_PIN[0], 1);

    for (int fanIndex = 0; fanIndex < 6; fanIndex++)
    {
        Fan InitiliseFan;
        switch (fanIndex)
        {
        case 0:
            InitiliseFan.GPIO_PWM_OUTPUT_PIN = Project_Definitions::GPIO_PWM_OUTPUT[fanIndex];
            InitiliseFan.GPIO_PWM_INPUT = 0;
            InitiliseFan.GPIO_FAN_TACHO = GPIO_TACHO_INPUTS[fanIndex];
            InitiliseFan.TEMP_SENSOR = 0;
            InitiliseFan.TACHO_FAN_SPEED = 0;
            InitiliseFan.LED_STATUS = false;

            InitiliseFan.GPIO_LED_PIN = FAN_LED_PIN[fanIndex];
            break;
        case 1:
            InitiliseFan.GPIO_PWM_OUTPUT_PIN = Project_Definitions::GPIO_PWM_OUTPUT[fanIndex];
            InitiliseFan.GPIO_PWM_INPUT = 1;
            InitiliseFan.GPIO_FAN_TACHO = GPIO_TACHO_INPUTS[fanIndex];
            InitiliseFan.TEMP_SENSOR = 0;
            InitiliseFan.TACHO_FAN_SPEED = 0;
            InitiliseFan.LED_STATUS = false;

            InitiliseFan.GPIO_LED_PIN = FAN_LED_PIN[fanIndex];
            break;
        case 2:
            InitiliseFan.GPIO_PWM_OUTPUT_PIN = Project_Definitions::GPIO_PWM_OUTPUT[fanIndex];
            InitiliseFan.GPIO_PWM_INPUT = 2;
            InitiliseFan.GPIO_FAN_TACHO = GPIO_TACHO_INPUTS[fanIndex];
            InitiliseFan.TEMP_SENSOR = 0;
            InitiliseFan.TACHO_FAN_SPEED = 0;
            InitiliseFan.LED_STATUS = false;

            InitiliseFan.GPIO_LED_PIN = FAN_LED_PIN[fanIndex];
            break;
        case 3:
            InitiliseFan.GPIO_PWM_OUTPUT_PIN = Project_Definitions::GPIO_PWM_OUTPUT[fanIndex];
            InitiliseFan.GPIO_PWM_INPUT = 0;
            InitiliseFan.GPIO_FAN_TACHO = GPIO_TACHO_INPUTS[fanIndex];
            InitiliseFan.TEMP_SENSOR = 0;
            InitiliseFan.TACHO_FAN_SPEED = 0;
            InitiliseFan.LED_STATUS = false;

            InitiliseFan.GPIO_LED_PIN = FAN_LED_PIN[fanIndex];
            break;
        case 4:
            InitiliseFan.GPIO_PWM_OUTPUT_PIN = Project_Definitions::GPIO_PWM_OUTPUT[fanIndex];
            InitiliseFan.GPIO_PWM_INPUT = 1;
            InitiliseFan.GPIO_FAN_TACHO = GPIO_TACHO_INPUTS[fanIndex];
            InitiliseFan.TEMP_SENSOR = 0;
            InitiliseFan.TACHO_FAN_SPEED = 0;
            InitiliseFan.LED_STATUS = false;

            InitiliseFan.GPIO_LED_PIN = FAN_LED_PIN[fanIndex];
            break;
        case 5:
            InitiliseFan.GPIO_PWM_OUTPUT_PIN = Project_Definitions::GPIO_PWM_OUTPUT[fanIndex];
            InitiliseFan.GPIO_PWM_INPUT = 2;
            InitiliseFan.GPIO_FAN_TACHO = GPIO_TACHO_INPUTS[fanIndex];
            InitiliseFan.TEMP_SENSOR = 0;
            InitiliseFan.TACHO_FAN_SPEED = 0;
            InitiliseFan.LED_STATUS = false;

            InitiliseFan.GPIO_LED_PIN = FAN_LED_PIN[fanIndex];
            break;
        }
        Fans[fanIndex] = InitiliseFan;
    }
}
