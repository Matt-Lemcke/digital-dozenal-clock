#ifndef FIRMWARE_INC_DOZ_CLOCK_H_
#define FIRMWARE_INC_DOZ_CLOCK_H_

#include <rtc_module.h>
#include "bitmaps.h"
#include "buzzer.h"
#include "clock_types.h"
#include "display.h"
#include "event_queue.h"
#include "gps.h"
#include "rtc_module.h"


typedef struct doz_clock_t
{
    Buzzer *buzzer;
    Display *display;
    Gps *gps;
    Rtc *rtc;

    EventId curr_event;

    bool alarm_set;
    bool alarm_triggered;
    bool timer_set;
    bool timer_triggered;
    bool show_error;
    uint8_t digit_sel;
    uint8_t digit_val;
    uint8_t error_code;
    uint32_t time_ms;
    uint32_t user_alarm_ms;
    uint32_t user_time_ms;
    uint32_t user_timer_ms;

    void (*error_handler)(void);
} DozClock;

// FSM Event Functions
void DozClock_Init(DozClock *ctx);
void DozClock_Update();
void DozClock_TimerCallback();  // Called from 6 Hz timer

#endif  // FIRMWARE_INC_DOZ_CLOCK_H_
