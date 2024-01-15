#ifndef FIRMWARE_INC_DOZ_CLOCK_H_
#define FIRMWARE_INC_DOZ_CLOCK_H_

#include "clock_types.h"
#include "display.h"
#include "gps.h"
#include "rtc.h"


typedef struct doz_clock_t
{
    Display *display;
    Gps *gps;
    Rtc *rtc;
} DozClock;

// FSM Event Functions
void DozClock_Init(DozClock *ctx);
void DozClock_Update();
void DozClock_BtnPress();
void DozClock_TimerCallback();

#endif  // FIRMWARE_INC_DOZ_CLOCK_H_
