#ifndef FIRMWARE_INC_DISPLAY_H_
#define FIRMWARE_INC_DISPLAY_H_

#include <stdint.h>
#include <clock_types.h>

#define MAX_CHARACTERS 8
#define MILLISECONDS_PER_DAY 86400000

typedef enum timeFormat{
    DEC24H,
    DIURNAL,
} TimeFormat;

typedef struct systime{
    TimeFormat format;
    uint32_t milliseconds;
} Time;

typedef struct display_t{
    uint8_t brightness;
} Display;

ClockStatus Display_Init(Display *self);
ClockStatus Display_UpdateTime(Display *self, Time *curr_time, char *output);

#endif  // FIRMWARE_INC_DISPLAY_H_