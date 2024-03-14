/*
 * gpio-buttons.h
 *
 *  Created on: Jan. 14, 2024
 *      Author: Matt L.
 */

#ifndef GPIO_BUTTONS_H_
#define GPIO_BUTTONS_H_

#include "clock_types.h"
#include "main.h"

#define NUM_BUTTONS 12

void Buttons_Init(void);
void Buttons_GpioCallback(uint16_t pin);
void Buttons_TimerCallback(uint32_t period_ms); // Call from <=6Hz timer

#endif /* GPIO_BUTTONS_H_ */
