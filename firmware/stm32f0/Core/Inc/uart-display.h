/*
 * uart-display.h
 *
 *  Created on: Jan 7, 2024
 *      Author: Matt
 */

#ifndef INC_UART_DISPLAY_H_
#define INC_UART_DISPLAY_H_

#include "main.h"
#include "chrono_protocol.h"

#include "test_display.h"

bool Esp8266Driver_Init(UART_HandleTypeDef *huart, uint32_t timeout_ms);
void Esp8266Driver_DisplayOff(void);
void Esp8266Driver_DisplayOn(void);
void Esp8266Driver_SetDisplayBrightness(uint8_t brightness);
void Esp8266Driver_SetBitmap(uint8_t region_id, uint8_t *bitmap);
void Esp8266Driver_SetColour(uint8_t region_id, uint8_t colour_id);
void Esp8266Driver_Show(uint8_t region_id);
void Esp8266Driver_Hide(uint8_t region_id);

#endif /* INC_UART_DISPLAY_H_ */
