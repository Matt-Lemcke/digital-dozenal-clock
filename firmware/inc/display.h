#ifndef FIRMWARE_INC_DISPLAY_H_
#define FIRMWARE_INC_DISPLAY_H_

#include <stdint.h>

#define MAX_CHARACTERS 8

typedef enum timeFormat_t{
    DEC24H,
    DIURNAL,
} TimeFormat;

typedef struct time_t{
    TimeFormat format;
    uint32_t milliseconds;
} Time;

typedef struct display_t{
    uint8_t brightness;
} Display;

void Display_Init(Display *self);
void Display_UpdateTime(Display *self, Time *curr_time);

#endif  // FIRMWARE_INC_DISPLAY_H_