/*
 * hub75-driver.h
 *
 *  Created on: Jan 26, 2024
 *      Author: lemck
 */

#ifndef INC_HUB75_DRIVER_H_
#define INC_HUB75_DRIVER_H_

#include "main.h"
#include "clock_types.h"

#define TOP_REGION_ID   0
#define MID_REGION_ID   1
#define BOT_REGION_ID   2

#define LARGE_REGION_SIZE 96
#define SMALL_REGION_SIZE 56

#define MAX_BRIGHTNESS  255

// Timer input requires 60*32Hz frequency
void HUB75_Init(SPI_HandleTypeDef *spi, TIM_HandleTypeDef *tim, uint32_t channel);
void HUB75_DisplayOff(void);
void HUB75_DisplayOn(void);
void HUB75_SetDisplayBrightness(uint8_t brightness);
void HUB75_SetBitmap(uint8_t region_id, uint8_t *bitmap);
void HUB75_SetColour(uint8_t region_id, Colour colour_id);
void HUB75_Show(uint8_t region_id);
void HUB75_Hide(uint8_t region_id);

// Call from 60*32 Hz timer callback
void HUB75_PwmStartPulse(void);

#endif /* INC_HUB75_DRIVER_H_ */
