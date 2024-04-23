/*
 * pwm-buzzer.h
 *
 *  Created on: Apr 11, 2024
 *      Author: lemck
 */

#ifndef INC_PWM_BUZZER_H_
#define INC_PWM_BUZZER_H_

#include "main.h"

#define OUTPUT_MAX  100
#define OUTPUT_MIN  0

void PKM22E_Init(TIM_HandleTypeDef *freq_tim, DAC_HandleTypeDef *hdac, uint32_t channel);
void PKM22E_VoltageOutput(uint32_t level);
void PKM22E_Start(void);
void PKM22E_Stop(void);
void PKM22E_FreqTimerCallback(void);

#endif /* INC_PWM_BUZZER_H_ */
