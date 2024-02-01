/*
 * hub75-driver.h
 *
 *  Created on: Jan 26, 2024
 *      Author: lemck
 */

#ifndef INC_HUB75_DRIVER_H_
#define INC_HUB75_DRIVER_H_

#include "main.h"

#define TOP_REGION_ID   1
#define MID_REGION_ID   2
#define BOT_REGION_ID   3

typedef enum
{
    Red     = 0b100,
    Green   = 0b010,
    Blue    = 0b001,
    Yellow  = 0b110,
    Magenta = 0b101,
    Cyan    = 0b011,
    White   = 0b111,
}Colour_Id;

void HUB75_Init(SPI_HandleTypeDef *spi);
void HUB75_Start(void);
void HUB75_TimerCallback(void);

void HUB75_DisplayOff(void);
void HUB75_DisplayOn(void);
void HUB75_SetDisplayBrightness(uint8_t brightness);
void HUB75_SetBitmap(uint8_t region_id, uint8_t *bitmap);
void HUB75_SetColour(uint8_t region_id, Colour_Id colour_id);
void HUB75_Show(uint8_t region_id);
void HUB75_Hide(uint8_t region_id);

#endif /* INC_HUB75_DRIVER_H_ */
