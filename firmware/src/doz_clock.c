#include "doz_clock.h"

#include <stdio.h>

#include "time_track.h"

#define TIMER_PERIOD_MS 167

typedef enum {
    STATE_INIT, STATE_24H, STATE_DIURNAL
} StateCode;

typedef struct state_t {
    StateCode state_code;

    void (*entry)(DozClock *ctx);
    void (*update)(DozClock *ctx);
    void (*exit)(DozClock *ctx);
    void (*onBtnPress)(DozClock *ctx);
    void (*timerCallback)(DozClock *ctx);
} State;

typedef struct state_machine_t {
    State *curr_state;
    DozClock *ctx;
} DozClockFSM;

static void Default_Entry(DozClock *ctx);
static void Default_Update(DozClock *ctx);
static void Default_Exit(DozClock *ctx);
static void Default_OnBtnPress(DozClock *ctx);
static void Default_TimerCallback(DozClock *ctx);
static void ClockStateInit_Entry(DozClock *ctx);
static void ClockStateInit_Update(DozClock *ctx);
static void ClockState24h_Entry(DozClock *ctx);
static void ClockState24h_Update(DozClock *ctx);
static void ClockState24h_OnBtnPress(DozClock *ctx);
static void ClockStateDiurnal_Entry(DozClock *ctx);
static void ClockStateDiurnal_Update(DozClock *ctx);
static void ClockStateDiurnal_OnBtnPress(DozClock *ctx);

static void transition(State *next);

static DozClockFSM g_clock_fsm = { 0 };
static TimeSources g_time_sources = { 0 };
uint32_t curr_time = 0, prev_time = 0;

static RtcTime reset_time = { .date = 20, .dow = 1, .month = 11, .year = 2023,
        .hr = 14, .min = 30, .sec = 0 };

// State definitions
State clock_init =
{
        .state_code = STATE_INIT,
        .entry = ClockStateInit_Entry,
        .exit = Default_Exit,
        .update = ClockStateInit_Update,
        .onBtnPress = Default_OnBtnPress,
        .timerCallback = Default_TimerCallback
};

State clock_24h =
{
        .state_code = STATE_24H,
        .entry = ClockState24h_Entry,
        .exit = Default_Exit,
        .update = ClockState24h_Update,
        .onBtnPress = ClockState24h_OnBtnPress,
        .timerCallback = Default_TimerCallback
};

State clock_diurnal =
{
        .state_code = STATE_DIURNAL,
        .entry = ClockStateDiurnal_Entry,
        .exit = Default_Exit,
        .update = ClockStateDiurnal_Update,
        .onBtnPress = ClockStateDiurnal_OnBtnPress,
        .timerCallback = Default_TimerCallback
};

// FSM Event Functions
void DozClock_Init(DozClock *ctx)
{
    g_clock_fsm.ctx = ctx;
    g_clock_fsm.curr_state = &clock_init;
    g_clock_fsm.curr_state->entry(g_clock_fsm.ctx);

    g_time_sources.gps = ctx->gps;
    g_time_sources.rtc = ctx->rtc;
    TimeTrack_Init(&g_time_sources);

    Rtc_SetTime(g_time_sources.rtc, &reset_time);
    curr_time = 0;
    prev_time = 0;
}
void DozClock_Update()
{
    g_clock_fsm.curr_state->update(g_clock_fsm.ctx);
}
void DozClock_BtnPress()
{
    g_clock_fsm.curr_state->onBtnPress(g_clock_fsm.ctx);
}
void DozClock_TimerCallback()
{
    g_clock_fsm.curr_state->timerCallback(g_clock_fsm.ctx);
}

// Generic State Functions
static void Default_Entry(DozClock *ctx)
{
    UNUSED(ctx);
}
static void Default_Update(DozClock *ctx)
{
    UNUSED(ctx);
    TimeTrack_Update();
}
static void Default_Exit(DozClock *ctx)
{
    UNUSED(ctx);
}
static void Default_OnBtnPress(DozClock *ctx)
{
    UNUSED(ctx);
}
static void Default_TimerCallback(DozClock *ctx)
{
    UNUSED(ctx);
    TimeTrack_PeriodicCallback(TIMER_PERIOD_MS);
}

// Init State Functions
static void ClockStateInit_Entry(DozClock *ctx)
{
    Display_Init(ctx->display);
}
static void ClockStateInit_Update(DozClock *ctx)
{
    TimeTrack_Update();
    transition(&clock_24h);
}

// 24H State Functions
static void ClockState24h_Entry(DozClock *ctx)
{
    Display_UpdateTime(ctx->display, curr_time, DEC24H);
}
static void ClockState24h_Update(DozClock *ctx)
{
    TimeTrack_GetTimeMs(&curr_time);
    if (curr_time != prev_time)
    {
        Display_UpdateTime(ctx->display, curr_time, DEC24H);
        prev_time = curr_time;
    }
    TimeTrack_Update();
}
static void ClockState24h_OnBtnPress(DozClock *ctx)
{
    UNUSED(ctx);
    transition(&clock_diurnal);
}

// Diurnal Functions
static void ClockStateDiurnal_Entry(DozClock *ctx)
{
    Display_UpdateTime(ctx->display, curr_time, DIURNAL);
}
static void ClockStateDiurnal_Update(DozClock *ctx)
{
    TimeTrack_GetTimeMs(&curr_time);
    if (curr_time != prev_time)
    {
        Display_UpdateTime(ctx->display, curr_time, DIURNAL);
        prev_time = curr_time;
    }
    TimeTrack_Update();
}
static void ClockStateDiurnal_OnBtnPress(DozClock *ctx)
{
    UNUSED(ctx);
    transition(&clock_24h);
}

// Helper functions
static void transition(State *next)
{
    g_clock_fsm.curr_state->exit(g_clock_fsm.ctx);
    g_clock_fsm.curr_state = next;
    g_clock_fsm.curr_state->entry(g_clock_fsm.ctx);
}

#ifdef NO_PLATFORM
int main(void)
{
    return 0;
}
#endif
