#ifndef PwmIn_H
#define PwmIn_H

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "PwmIn.h"
#include "PwmIn.pio.h"

// class that reads PWM pulses on max 4 pins
class PwmIn
{
public:
    // PIO instance number
    int PIOinstance;
    // List of pins
    uint pin_list[4];
    // Number of pins
    uint num_of_pins;
    // constructor
    void ConstructPwmIn();
    // read pulsewidth and period for one pulse
    void read_PWM(float *readings, uint pin);
    // read only the pulsewidth
    float read_PW(uint pin);
    // read only the duty cycle
    float read_DC(uint pin);
    // read only the period
    float read_P(uint pin);
    // Variables
    uint32_t pulsewidth[4], period[4];
    // Enable IRQ
    void Enable_IRQ(bool enable);
    // Number of readouts before the fan value resets to 0
    int ValueTimeout = 5;

    // PwmIn Instances
    // PwmIn Instance 0
    static PwmIn PioPwm0;
    // PwmIn Instance 1
    static PwmIn PioPwm1;

private:
    // set the irq handler
    int currentCounterValue[4];
    uint32_t LastReadOutPeriod[4];

    static void pio_irq_handler_pio0()
    {
        int state_machine = -1;
        // check which IRQ was raised:
        for (int i = 0; i < 4; i++)
        {
            if (pio0_hw->irq & 1 << i)
            {
                // clear interrupt
                pio0_hw->irq = 1 << i;
                // read pulse width from the FIFO
                PioPwm0.pulsewidth[i] = pio_sm_get(pio0, i);
                // read low period from the FIFO
                PioPwm0.period[i] = pio_sm_get(pio0, i);
                // clear interrupt
                pio0_hw->irq = 1 << i;
            }
        }
    }
    static void pio_irq_handler_pio1()
    {
        int state_machine = -1;
        // check which IRQ was raised:
        for (int i = 0; i < 4; i++)
        {
            if (pio1_hw->irq & 1 << i)
            {
                // clear interrupt
                pio1_hw->irq = 1 << i;
                // read pulse width from the FIFO
                PioPwm1.pulsewidth[i] = pio_sm_get(pio1, i);
                // read low period from the FIFO
                PioPwm1.period[i] = pio_sm_get(pio1, i);
                // clear interrupt
                pio1_hw->irq = 1 << i;
            }
        }
    }
};

#endif