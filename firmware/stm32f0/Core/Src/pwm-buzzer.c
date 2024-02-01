/*
 * pwm-buzzer.c
 *
 *  Created on: Jan 7, 2024
 *      Author: Matt
 */

#include "pwm-buzzer.h"

#define CCR_COEFF 90

TIM_HandleTypeDef *pwm_timer;
uint32_t pwm_channel;

void PKM22E_Init(TIM_HandleTypeDef *htim, uint32_t channel)
{
    pwm_timer = htim;
    pwm_channel = channel;
    PKM22E_SetDutyCyle(DC_MIN);
}

void PKM22E_SetDutyCyle(uint32_t dc)
{
    if(dc < DC_MIN || dc > DC_MAX)
    {
        return;
    }
    pwm_timer->Instance->CCR1 = dc*CCR_COEFF;
}
void PKM22E_StartPwm(void)
{
    HAL_TIM_PWM_Start(pwm_timer, pwm_channel);
}
void PKM22E_StopPwm(void)
{
    HAL_TIM_PWM_Stop(pwm_timer, pwm_channel);
}
