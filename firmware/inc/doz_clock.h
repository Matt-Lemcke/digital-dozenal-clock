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
#include "time_track.h"
#include <math.h>

#define TIMER_PERIOD_MS  167
#define MAX_DIGITS       7
#define PM_12H_MS       (43200000 - 1)

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
    bool timer_alarm_displayed; // 0: timer, 1: alarm
    uint8_t digit_sel;
    uint8_t digit_vals[MAX_DIGITS];
    uint8_t diurn_radix_pos;
    uint8_t semi_diurn_radix_pos;
    ClockStatus error_code;
    uint32_t time_ms;
    uint32_t user_alarm_ms;
    uint32_t user_time_ms;
    uint32_t user_timer_ms;

    void (*error_handler)(void);
} DozClock;

typedef enum doz_clock_state_code_t {
    STATE_INIT,
    STATE_SET_TIME,
    STATE_SET_ALARM,
    STATE_SET_TIMER,
    STATE_IDLE_DISP_ON,
    STATE_IDLE_DISP_OFF,
    STATE_ALARM_TIMER_DISP_ON,
    STATE_ALARM_TIMER_DISP_OFF,
    NUM_STATES
} DozClockStateCode;

typedef struct doz_clock_state_t {
    DozClockStateCode state_code;

    void (*entry)(DozClock *ctx);
    void (*update)(DozClock *ctx);
    void (*exit)(DozClock *ctx);
} DozClockState;

typedef struct doz_clock_state_machine_t {
    DozClockState *curr_state;
    DozClock *ctx;
} DozClockFSM;

// FSM Event Functions
void DozClock_Init(DozClock *ctx);
void DozClock_Update();
void DozClock_TimerCallback();  // Called from 6 Hz timer
void msToTrad(uint32_t time_ms, uint8_t *hr_24, uint8_t *min, uint8_t *sec);
void msToDiurn(uint32_t time_ms, uint8_t *digit1, uint8_t *digit2, uint8_t *digit3, uint8_t *digit4, uint8_t *digit5);
void msToSemiDiurn(uint32_t time_ms, uint8_t *digit1, uint8_t *digit2, uint8_t *digit3, uint8_t *digit4, uint8_t *digit5);

#endif  // FIRMWARE_INC_DOZ_CLOCK_H_
