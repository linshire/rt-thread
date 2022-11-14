/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-11-12     linshrie     first version
 */

#include <Arduino.h>
#include <board.h>
#include "pins_arduino.h"

/*
 * {Arduino Pin, RT-Thread Pin [, Device Name, Channel]}
 * [] means optional
 * Digital pins must NOT give the device name and channel.
 * Analog pins MUST give the device name and channel(ADC, PWM or DAC).
 * Arduino Pin must keep in sequence.
 */
const pin_map_t pin_map_table[]=
{
    {D0, GET_PIN(A,1), "uart4"},        /* Serial2-TX */
    {D1, GET_PIN(A,0), "uart4"},        /* Serial2-RX */
    {D2, GET_PIN(D,14)},
    {D3, GET_PIN(E,6), "pwm3", 4},      /* PWM3 CH4 */
    {D4, GET_PIN(D,11)},
    {D5, GET_PIN(E,5), "pwm3", 3},      /* PWM3 CH3 */
    {D6, GET_PIN(E,4), "pwm3", 2},      /* PWM3 CH2 */
    {D7, GET_PIN(E,0)},
    {D8, GET_PIN(C,11)},
    {D9, GET_PIN(C,10)},
    {D10, GET_PIN(D,0)},
    {D11, GET_PIN(C,1)},
    {D12, GET_PIN(D,3)},
    {D13, GET_PIN(A,9)},
    {D14, GET_PIN(C,1), "i2c2"},        /* I2C-SCL (Wire) */
    {D15, GET_PIN(C,0), "i2c2"},        /* I2C-SDA (Wire) */
    {D16, GET_PIN(C,7)},
    {D17, GET_PIN(C,12)},
    {D18, GET_PIN(D,1)},
    {D19, GET_PIN(D,7)},
    {A0, GET_PIN(C,2), "adc1", 3},      /* ADC */
    {A1, GET_PIN(C,3), "adc1", 4}       /* ADC */
};
