/*
 * dac-buzzer.h
 *
 *  Created on: Apr 22, 2024
 *      Author: lemck
 */

#ifndef INC_DAC_BUZZER_H_
#define INC_DAC_BUZZER_H_

#include "main.h"

#define OUTPUT_MAX  100
#define OUTPUT_MIN  0

void PKM22E_DAC_Init(TIM_HandleTypeDef *freq_tim, DAC_HandleTypeDef *hdac, uint32_t channel);
void PKM22E_DAC_OutputLevel(uint32_t level);
void PKM22E_DAC_Start(void);
void PKM22E_DAC_Stop(void);
void PKM22E_DAC_FreqTimerCallback(void);

#endif /* INC_DAC_BUZZER_H_ */
