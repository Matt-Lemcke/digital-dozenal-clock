/*
 * adc-light-sens.h
 *
 *  Created on: Jan 13, 2024
 *      Author: Matt L.
 */

#ifndef INC_ADC_LIGHT_SENS_H_
#define INC_ADC_LIGHT_SENS_H_

#include "main.h"
#include "clock_types.h"

void LightSens_Init(ADC_HandleTypeDef *hadc, uint16_t threshold);
void LightSens_AdcSampleCallback(void);

#endif /* INC_ADC_LIGHT_SENS_H_ */
