/*
 * gpio-display.h
 *
 *  Created on: Jan 1, 2024
 *      Author: lemck
 */

#ifndef INC_GPIO_DISPLAY_H_
#define INC_GPIO_DISPLAY_H_

#include "main.h"

#define PIXEL_COLS                  64
#define PIXEL_ROWS                  32
#define HORIZONTAL_BYTES            (PIXEL_COLS/8)
#define TOTAL_BYTES                 (PIXEL_COLS * PIXEL_ROWS / 8)

#define PIXELS_COUNT                (PIXEL_COLS * PIXEL_ROWS)
#define PIXELS_COUNT_IN_BYTES       (PIXELS_COUNT/8)
#define HORIZONTAL_PIXELS_IN_BYTES  (PIXEL_COLS/8)

typedef enum {
    PinRed1,
    PinGreen1,
    PinBlue1,
    PinRed2,
    PinGreen2,
    PinBlue2,
    PinADDRA,
    PinADDRB,
    PinADDRC,
    PinADDRD,
    PinADDRE,
    PinCLK,
    PinLATCH,
    PinOE

}HUB75EPin;

typedef enum {
    Red,
    Green,
    Blue,
    Yellow, //RG
    Pink, //RB
    Cyan, //GB
    White, //RGB
    Black //All off
}HUB75EDisplayColor;

typedef enum {
    BrightnessLevel1,
    BrightnessLevel2,
    BrightnessLevel3,
    BrightnessLevel4,
    BrightnessLevel5
}HUB75EDisplayBrightnessLevel;

typedef enum {
    HUB75EAddressingModeABCDE,
    HUB75EAddressingModeAC
}HUB75EAddressingMode;

void HUB75E_Init(void);



void HUB75E_clearDisplayBuffer();
void HUB75E_setDisplayBuffer(unsigned char pixels[]);
void HUB75E_DisplayOFF();
void HUB75E_displayBufferPixels();
void HUB75E_setDisplayBrightness(HUB75EDisplayBrightnessLevel b);
void HUB75E_setDisplayColor(HUB75EDisplayColor c);
void HUB75E_setAddressingMode(HUB75EAddressingMode addrMode);

#endif /* INC_GPIO_DISPLAY_H_ */
