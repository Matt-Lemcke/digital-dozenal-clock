#ifndef FIRMWARE_INC_DISPLAY_H_
#define FIRMWARE_INC_DISPLAY_H_

#include "clock_types.h"

#define MAX_CHARACTERS       8
#define MILLISECONDS_PER_DAY 86400000

typedef enum timeFormat
{
    DEC24H,
    DIURNAL,
} TimeFormat;

typedef struct display_t
{
    uint8_t brightness;

    void (*sendString)(char *st);
    void (*clear)();
} Display;

ClockStatus Display_Init(Display *self);
ClockStatus Display_UpdateTime(Display *self, uint32_t time_ms,
                               TimeFormat format);

#endif  // FIRMWARE_INC_DISPLAY_H_
