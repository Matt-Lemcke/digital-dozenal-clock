#include "doz_clock.h"

#include "time_track.h"

#define TIMER_PERIOD_MS 167

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

static void Default_Entry(DozClock *ctx);
static void Default_Update(DozClock *ctx);
static void Default_Exit(DozClock *ctx);
static void Init_Entry(DozClock *ctx);
static void Init_Update(DozClock *ctx);
static void Init_Exit(DozClock *ctx);
static void IdleDispOn_Entry(DozClock *ctx);
static void IdleDispOn_Update(DozClock *ctx);
static void IdleDispOff_Entry(DozClock *ctx);
static void IdleDispOff_Update(DozClock *ctx);
static void IdleDispOff_Exit(DozClock *ctx);

static void transition(State *next);
static void process_event();

static ExternVars clock_vars = { 0 };
static DozClockFSM g_clock_fsm = { 0 };
static uint8_t timer_delay = 0;

static TimeFormats trad_format_list[] = {TRAD_24H, TRAD_12H};
static TimeFormats doz_format_list[] = {DOZ_SEMI, DOZ_DRN4, DOZ_DRN5};

static trad_index = 0, doz_index = 0;

// State definitions
static State s_init =
{
    .state_code = STATE_INIT,
    .entry      = Init_Entry,
    .update     = Init_Update,
    .exit       = Init_Exit,
};
static State s_idle_disp_on =
{
    .state_code = STATE_IDLE_DISP_ON,
    .entry      = IdleDispOn_Entry,
    .update     = IdleDispOn_Update,
    .exit       = Default_Exit,
};
static State s_idle_disp_off =
{
    .state_code = STATE_IDLE_DISP_OFF,
    .entry      = IdleDispOff_Entry,
    .update     = IdleDispOff_Update,
    .exit       = IdleDispOff_Exit,
};

// FSM Event Functions
void DozClock_Init(DozClock *ctx)
{
    clock_vars.alarm_set        = &ctx->alarm_set;
    clock_vars.alarm_triggered  = &ctx->alarm_triggered;
    clock_vars.timer_set        = &ctx->timer_set;
    clock_vars.timer_triggered  = &ctx->timer_triggered;
    clock_vars.show_error       = &ctx->show_error;
    clock_vars.digit_sel        = &ctx->digit_sel;
    clock_vars.digit_val        = &ctx->digit_val;
    clock_vars.error_code       = &ctx->error_code;
    clock_vars.time_ms          = &ctx->time_ms;
    clock_vars.user_alarm_ms    = &ctx->user_alarm_ms;
    clock_vars.user_time_ms     = &ctx->user_time_ms;
    clock_vars.user_timer_ms    = &ctx->user_timer_ms;

    *clock_vars.alarm_set        = 0;
    *clock_vars.alarm_triggered  = 0;
    *clock_vars.timer_set        = 0;
    *clock_vars.timer_triggered  = 0;
    *clock_vars.show_error       = 0;
    *clock_vars.digit_sel        = 0;
    *clock_vars.digit_val        = 0;
    *clock_vars.error_code       = 0;
    *clock_vars.time_ms          = 0;
    *clock_vars.user_alarm_ms    = 0;
    *clock_vars.user_time_ms     = 0;
    *clock_vars.user_timer_ms    = 0;

    ctx->curr_event = E_NONE;

    g_clock_fsm.ctx = ctx;
    g_clock_fsm.curr_state = &s_init;
    g_clock_fsm.curr_state->entry(g_clock_fsm.ctx);
}

void DozClock_Update()
{
    TimeTrack_Update();
    g_clock_fsm.curr_state->update(g_clock_fsm.ctx);
    Display_Update();
    if (EventQ_GetEvent(&g_clock_fsm.ctx->curr_event) == CLOCK_OK)
    {
        process_event();
    }
}

// Called from 6 Hz timer
void DozClock_TimerCallback()
{
    TimeTrack_PeriodicCallback(TIMER_PERIOD_MS);
    if (timer_delay == 0)
    {
        // Called as 2 Hz timer
        Display_PeriodicCallback();
    }
    timer_delay = (timer_delay + 1) % 3;
}

// Generic State Functions
void Default_Entry(DozClock *ctx)
{
    UNUSED(ctx);
}
void Default_Update(DozClock *ctx)
{
    UNUSED(ctx);
}
void Default_Exit(DozClock *ctx)
{
    UNUSED(ctx);
}

// Init State Functions
void Init_Entry(DozClock *ctx)
{
    // Software driver initialization
    EventQ_Init();
    if (Buzzer_Init(ctx->buzzer) != CLOCK_OK)
    {
        ctx->error_code = SFWR_INIT;
        ctx->error_handler();
    } 
    if (Gps_Init(ctx->gps) != CLOCK_OK)
    {
        ctx->error_code = SFWR_INIT;
        ctx->error_handler();
    }
    if (Rtc_Init(ctx->rtc) != CLOCK_OK)
    {
        ctx->error_code = SFWR_INIT;
        ctx->error_handler();
    }
    if (Display_Init(ctx->display, &clock_vars) != CLOCK_OK)
    {
        ctx->error_code = DISP_INIT;
        ctx->error_handler();
    }
    if (TimeTrack_Init() != CLOCK_OK)
    {
        ctx->error_code = TIME_INIT;
        ctx->error_handler();
    }
}
void Init_Update(DozClock *ctx)
{
    UNUSED(ctx);
    Display_SetFormat(TRAD_24H);
    if (TimeTrack_SyncToRtc() != CLOCK_OK)
    {
        ctx->error_code = TIME_INIT;
        ctx->error_handler();
    }
    TimeTrack_GetTimeMs(&ctx->time_ms);
    transition(&s_idle_disp_on);
}
void Init_Exit(DozClock *ctx)
{
    UNUSED(ctx);
    Display_On();
}
void IdleDispOn_Entry(DozClock *ctx)
{
    UNUSED(ctx);
    Display_ShowTime();
}
void IdleDispOn_Update(DozClock *ctx)
{
    UNUSED(ctx);
    TimeTrack_GetTimeMs(&ctx->time_ms);
}
void IdleDispOff_Entry(DozClock *ctx)
{
    UNUSED(ctx);
    Display_Off();
}
void IdleDispOff_Update(DozClock *ctx)
{
    UNUSED(ctx);
    TimeTrack_GetTimeMs(&ctx->time_ms);
}
void IdleDispOff_Exit(DozClock *ctx)
{
    UNUSED(ctx);
    Display_On();
}

// Helper functions
static void transition(State *next)
{
    g_clock_fsm.curr_state->exit(g_clock_fsm.ctx);
    g_clock_fsm.curr_state = next;
    g_clock_fsm.curr_state->entry(g_clock_fsm.ctx);
}

static void process_event()
{
    if (g_clock_fsm.curr_state->state_code == STATE_IDLE_DISP_ON)
    {
        switch(g_clock_fsm.ctx->curr_event)
        {
            case E_DISPLAY_SHORT:
                Display_ToggleMode();
                break;
            case E_DISPLAY_LONG:
                transition(&s_idle_disp_off);
                break;
            case E_DOZ_SHORT:
                doz_index = (doz_index + 1) % 3;
                Display_SetFormat(doz_format_list[doz_index]);
                break;
            case E_TRAD_SHORT:
                trad_index = (trad_index + 1) % 2;
                Display_SetFormat(trad_format_list[trad_index]);
                break;
            case E_ROOM_DARK:
                Display_SetBrightness(LOW_BRIGHTNESS);
                break;
            case E_ROOM_LIGHT:
                Display_SetBrightness(HIGH_BRIGHTNESS);
                break;
            default:
                break;
        }
    }
    else if (g_clock_fsm.curr_state->state_code == STATE_IDLE_DISP_OFF)
    {
        switch(g_clock_fsm.ctx->curr_event)
        {
            case E_DISPLAY_LONG:
                transition(&s_idle_disp_on);
                break;
            case E_ROOM_DARK:
                Display_SetBrightness(LOW_BRIGHTNESS);
                break;
            case E_ROOM_LIGHT:
                Display_SetBrightness(HIGH_BRIGHTNESS);
                break;
            default:
                break;
        }
    }

    g_clock_fsm.ctx->curr_event = E_NONE;
}

#ifdef NO_PLATFORM
int main(void)
{
    return 0;
}
#endif

