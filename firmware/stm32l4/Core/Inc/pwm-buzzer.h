/*
 * pwm-buzzer.h
 *
 *  Created on: Jan 7, 2024
 *      Author: Matt
 */

#ifndef INC_PWM_BUZZER_H_
#define INC_PWM_BUZZER_H_

#include "main.h"

#define OUTPUT_MAX  100
#define OUTPUT_MIN  0

void PKM22E_PWM_Init(TIM_HandleTypeDef *htim, uint32_t channel);
void PKM22E_PWM_OutputLevel(uint32_t level);
void PKM22E_PWM_Start(void);
void PKM22E_PWM_Stop(void);


#endif /* INC_PWM_BUZZER_H_ */
