/*
 * pwm-buzzer.c
 *
 *  Created on: Apr 11, 2024
 *      Author: lemck
 */


#include "pwm-buzzer.h"

#define DAC_MAX_VAL 4095

static TIM_HandleTypeDef *tim;
static DAC_HandleTypeDef *dac;
static uint32_t dac_channel;
static uint32_t dac_value = 0;
static state = 0;

void PKM22E_Init(TIM_HandleTypeDef *freq_tim, DAC_HandleTypeDef *hdac, uint32_t channel)
{
    tim = freq_tim;
    dac = hdac;
    dac_channel = channel;
    PKM22E_VoltageOutput(OUTPUT_MIN);
}

void PKM22E_VoltageOutput(uint32_t level)
{
    if(level < OUTPUT_MIN || level > OUTPUT_MAX)
    {
        return;
    }
    dac_value = DAC_MAX_VAL*level/OUTPUT_MAX;
}

void PKM22E_Start(void)
{
    HAL_DAC_Start(dac, dac_channel);
    HAL_TIM_Base_Start_IT(tim);
}

void PKM22E_Stop(void)
{
    HAL_DAC_Stop(dac, dac_channel);
    HAL_TIM_Base_Stop_IT(tim);
}

void PKM22E_FreqTimerCallback(void)
{
    if (state)
    {
        HAL_DAC_SetValue(dac, dac_channel, DAC_ALIGN_12B_R, 0);
        state = 0;
    }
    else
    {
        HAL_DAC_SetValue(dac, dac_channel, DAC_ALIGN_12B_R, dac_value);
        state = 1;
    }
}
