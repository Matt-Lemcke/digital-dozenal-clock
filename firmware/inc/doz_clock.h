#ifndef FIRMWARE_INC_DOZ_CLOCK_H_
#define FIRMWARE_INC_DOZ_CLOCK_H_

#include "clock_types.h"
#include "display.h"
#include "rtc.h"

// States
typedef enum
{
    STATE_INIT,
    STATE_24H,
    STATE_DIURNAL
} StateCode;

typedef struct doz_clock_t
{
    void (*getTime)(void);
    Display *display;
    Rtc *rtc;
} DozClock;

typedef struct state_t
{
    StateCode state_code;

    void (*entry)(DozClock *ctx);
    void (*update)(DozClock *ctx);
    void (*exit)(DozClock *ctx);
    void (*onBtnPress)(DozClock *ctx);
} State;

typedef struct state_machine_t
{
    State *curr_state;
    DozClock *ctx;
} DozClockFSM;

// FSM Event Functions
void DozClock_Init(DozClock *ctx);
void DozClock_Update();
void DozClock_BtnPress();

#endif  // FIRMWARE_INC_DOZ_CLOCK_H_
