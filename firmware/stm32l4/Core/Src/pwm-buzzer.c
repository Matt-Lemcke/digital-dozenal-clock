/*
 * pwm-buzzer.c
 *
 *  Created on: Jan 7, 2024
 *      Author: Matt
 */

#include "pwm-buzzer.h"

#define CCR_COEFF ((pwm_timer->Instance->ARR)/200)

TIM_HandleTypeDef *pwm_timer;
uint32_t pwm_channel;

void PKM22E_PWM_Init(TIM_HandleTypeDef *htim, uint32_t channel)
{
    pwm_timer = htim;
    pwm_channel = channel;
    PKM22E_PWM_OutputLevel(OUTPUT_MIN);
}

void PKM22E_PWM_OutputLevel(uint32_t level)
{
    if(level < OUTPUT_MIN || level > OUTPUT_MAX)
    {
        return;
    }
    pwm_timer->Instance->CCR1 = CCR_COEFF*level;
}
void PKM22E_PWM_Start(void)
{
    HAL_TIM_PWM_Start(pwm_timer, pwm_channel);
}
void PKM22E_PWM_Stop(void)
{
    HAL_TIM_PWM_Stop(pwm_timer, pwm_channel);
}
