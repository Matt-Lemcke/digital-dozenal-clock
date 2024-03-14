/*
 * pwm-buzzer.h
 *
 *  Created on: Jan 7, 2024
 *      Author: Matt
 */

#ifndef INC_PWM_BUZZER_H_
#define INC_PWM_BUZZER_H_

#include "main.h"

#define DC_MAX  100
#define DC_MIN  0

void PKM22E_Init(TIM_HandleTypeDef *htim, uint32_t channel);
void PKM22E_SetDutyCyle(uint32_t dc);
void PKM22E_StartPwm(void);
void PKM22E_StopPwm(void);


#endif /* INC_PWM_BUZZER_H_ */
