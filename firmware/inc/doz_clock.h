#ifndef FIRMWARE_INC_DOZ_CLOCK_H_
#define FIRMWARE_INC_DOZ_CLOCK_H_

#include "bitmaps.h"
#include "buzzer.h"
#include "clock_types.h"
#include "display.h"
#include "event_queue.h"
#include "gps.h"
#include "rtc.h"
#include "time_track.h"

#define TIMER_PERIOD_MS 167

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
    uint8_t digit_vals[7];
    uint8_t error_code;
    uint32_t time_ms;
    uint32_t user_alarm_ms;
    uint32_t user_time_ms;
    uint32_t user_timer_ms;

    void (*error_handler)(void);
} DozClock;

enum {
    NO_ERROR,
    SFWR_INIT,
    DISP_INIT,
    TIME_INIT
} error_codes;

typedef enum {
    STATE_INIT,
    STATE_SET_TIME,
    STATE_SET_ALARM,
    STATE_SET_TIMER,
    STATE_IDLE_DISP_ON,
    STATE_IDLE_DISP_OFF,
    STATE_ALARM_DISP_ON,
    STATE_TIMER_DISP_ON,
    STATE_ALARM_TIMER_DISP_OFF,
    NUM_STATES
} StateCode;

typedef struct state_t {
    StateCode state_code;

    void (*entry)(DozClock *ctx);
    void (*update)(DozClock *ctx);
    void (*exit)(DozClock *ctx);
} State;

typedef struct state_machine_t {
    State *curr_state;
    DozClock *ctx;
} DozClockFSM;

// FSM Event Functions
void DozClock_Init(DozClock *ctx);
void DozClock_Update();
void DozClock_TimerCallback();  // Called from 6 Hz timer

#endif  // FIRMWARE_INC_DOZ_CLOCK_H_
