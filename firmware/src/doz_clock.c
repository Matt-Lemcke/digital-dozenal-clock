#include "doz_clock.h"

static void (*state_event_map[NUM_STATES][NUM_EVENTS]) (void);

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
static void SetAlarm_Entry(DozClock *ctx);
static void SetAlarm_Update(DozClock *ctx);
static void SetAlarm_Exit(DozClock *ctx);
static void SetTimer_Entry(DozClock *ctx);
static void SetTimer_Update(DozClock *ctx);
static void SetTimer_Exit(DozClock *ctx);
static void SetTime_Entry(DozClock *ctx);
static void SetTime_Update(DozClock *ctx);
static void SetTime_Exit(DozClock *ctx);
static void AlarmTimerDispOn_Entry(DozClock *ctx);
static void AlarmTimerDispOn_Update(DozClock *ctx);
static void AlarmTimerDispOn_Exit(DozClock *ctx);
static void AlarmTimerDispOff_Entry(DozClock *ctx);
static void AlarmTimerDispOff_Update(DozClock *ctx);
static void AlarmTimerDispOff_Exit(DozClock *ctx);

static void transition(State *next);
static void process_event();
static void state_event_map_init();

static ExternVars clock_vars = { 0 };
static DozClockFSM g_clock_fsm = { 0 };
static uint8_t timer_delay = 0;

static TimeFormats trad_format_list[] = {TRAD_24H, TRAD_12H};
static TimeFormats doz_format_list[] = {DOZ_SEMI, DOZ_DRN4, DOZ_DRN5};
static TimeFormats curr_format;

static uint8_t trad_index = 0, doz_index = 0;
static uint8_t displayOn = 1;
static uint8_t cancel_pressed = 0;

static RtcTime demo_reset = {
        .hr = 17,
        .min = 34,
        .sec = 0
};

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
static State s_set_alarm =
{
    .state_code = STATE_SET_ALARM,
    .entry      = SetAlarm_Entry,
    .update     = SetAlarm_Update,
    .exit       = SetAlarm_Exit,
};
static State s_set_timer =
{
    .state_code = STATE_SET_TIMER,
    .entry      = SetTimer_Entry,
    .update     = SetTimer_Update,
    .exit       = SetTimer_Exit,
};
static State s_set_time =
{
    .state_code = STATE_SET_TIME,
    .entry      = SetTime_Entry,
    .update     = SetTime_Update,
    .exit       = SetTime_Exit,
};
static State s_alarm_timer_disp_on =
{
    .state_code = STATE_ALARM_TIMER_DISP_ON,
    .entry      = AlarmTimerDispOn_Entry,
    .update     = AlarmTimerDispOn_Update,
    .exit       = AlarmTimerDispOn_Exit,
};
static State s_alarm_timer_disp_off =
{
    .state_code = STATE_ALARM_TIMER_DISP_OFF,
    .entry      = AlarmTimerDispOff_Entry,
    .update     = AlarmTimerDispOff_Update,
    .exit       = AlarmTimerDispOff_Exit,
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
    clock_vars.digit_vals       = ctx->digit_vals;
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
    clock_vars.digit_vals[0]     = 0;
    clock_vars.digit_vals[1]     = 0;
    clock_vars.digit_vals[2]     = 0;
    clock_vars.digit_vals[3]     = 0;
    clock_vars.digit_vals[4]     = 0;
    clock_vars.digit_vals[5]     = 0;
    clock_vars.digit_vals[6]     = 0;
    *clock_vars.error_code       = 0;
    *clock_vars.time_ms          = 0;
    *clock_vars.user_alarm_ms    = 0;
    *clock_vars.user_time_ms     = 0;
    *clock_vars.user_timer_ms    = 0;

    state_event_map_init();

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
    TimeTrack_GetTimeMs(&ctx->time_ms);
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
void SetAlarm_Entry(DozClock *ctx)
{
    ctx->digit_sel = 0;
    ctx->user_alarm_ms = 0;
    g_clock_fsm.ctx->alarm_set = false;
    Display_SetAlarm();
}
void SetAlarm_Update(DozClock *ctx)
{
    if (curr_format == TRAD_24H || curr_format == TRAD_12H) {

        ctx->user_alarm_ms = (uint32_t) (10*ctx->digit_vals[4] + ctx->digit_vals[5]) * 1000 +   // sec
                             (uint32_t) (10*ctx->digit_vals[2] + ctx->digit_vals[3]) * 60000;   // min

        if (curr_format == TRAD_24H) {
            ctx->user_alarm_ms += (uint32_t) (10*ctx->digit_vals[0] + ctx->digit_vals[1]) * 3600000;
        } else { // TRAD_12H
            if (ctx->digit_vals[6] == 1 && (10*ctx->digit_vals[0] + ctx->digit_vals[1]) != 12) // PM && hour != 12
                ctx->user_alarm_ms += (uint32_t) (10*ctx->digit_vals[0] + ctx->digit_vals[1] + 12) * 3600000;
            else
                ctx->user_alarm_ms += (uint32_t) (10*ctx->digit_vals[0] + ctx->digit_vals[1]) * 3600000;
        }

    } else if (curr_format == DOZ_SEMI) {

        ctx->user_alarm_ms = (uint32_t) ctx->digit_vals[0] * 43200000 +
                             (uint32_t) ctx->digit_vals[1] * 3600000 +
                             (uint32_t) ctx->digit_vals[2] * 300000 +
                             (uint32_t) ctx->digit_vals[3] * 25000 +
                             (uint32_t) (ctx->digit_vals[4] * 25000) / 12;

    } else { // DOZ_DRN4 || DOZ_DRN5

        ctx->user_alarm_ms = (uint32_t) ctx->digit_vals[0] * 7200000 +
                             (uint32_t) ctx->digit_vals[1] * 600000 +
                             (uint32_t) ctx->digit_vals[2] * 50000 +
                             (uint32_t) (ctx->digit_vals[3] * 25000) / 6;

        if (curr_format == DOZ_DRN5)
            ctx->user_alarm_ms += (uint32_t) (ctx->digit_vals[4] * 25000) / 72;
    }
}
void SetAlarm_Exit(DozClock *ctx)
{
    if (cancel_pressed) {
        cancel_pressed = 0;
        return;
    }

    RtcTime alarmTime;
    msToRtcTime(ctx->user_alarm_ms, &alarmTime);
    Rtc_SetAlarm(&alarmTime, ALARM);
    g_clock_fsm.ctx->alarm_set = true;
}
void SetTimer_Entry(DozClock *ctx)
{
    ctx->digit_sel = 0;
    ctx->user_timer_ms = 0;
    g_clock_fsm.ctx->timer_set = false;
    Display_SetTimer();
}
void SetTimer_Update(DozClock *ctx)
{
    if (curr_format == TRAD_24H || curr_format == TRAD_12H) {
        ctx->user_timer_ms = (uint32_t) (10*ctx->digit_vals[4] + ctx->digit_vals[5]) * 1000 +
                             (uint32_t) (10*ctx->digit_vals[2] + ctx->digit_vals[3]) * 60000;
        if (curr_format == TRAD_24H) {
            ctx->user_timer_ms += (uint32_t) (10*ctx->digit_vals[0] + ctx->digit_vals[1]) * 3600000;
        } else { // TRAD_12H
            if (ctx->digit_vals[6] == 1 && (10*ctx->digit_vals[0] + ctx->digit_vals[1]) != 12) // PM && hour != 12
                ctx->user_timer_ms += (uint32_t) (10*ctx->digit_vals[0] + ctx->digit_vals[1] + 12) * 3600000;
            else
                ctx->user_timer_ms += (uint32_t) (10*ctx->digit_vals[0] + ctx->digit_vals[1]) * 3600000;
        }
    } else if (curr_format == DOZ_SEMI) {
        ctx->user_timer_ms = (uint32_t) ctx->digit_vals[0] * 43200000 +
                             (uint32_t) ctx->digit_vals[1] * 3600000 +
                             (uint32_t) ctx->digit_vals[2] * 300000 +
                             (uint32_t) ctx->digit_vals[3] * 25000 +
                             (uint32_t) (ctx->digit_vals[4] * 25000) / 12;
    } else {
        ctx->user_timer_ms = (uint32_t) ctx->digit_vals[0] * 7200000 +
                             (uint32_t) ctx->digit_vals[1] * 600000 +
                             (uint32_t) ctx->digit_vals[2] * 50000 +
                             (uint32_t) (ctx->digit_vals[3] * 25000) / 6;
        if (curr_format == DOZ_DRN5)
            ctx->user_timer_ms += (uint32_t) (ctx->digit_vals[4] * 25000) / 72;
    } 
}
void SetTimer_Exit(DozClock *ctx)
{
    if (cancel_pressed) {
        cancel_pressed = 0;
        return;
    }

    RtcTime timerTime;
    msToRtcTime(ctx->user_timer_ms, &timerTime);
    Rtc_SetAlarm(&timerTime, TIMER);
    g_clock_fsm.ctx->timer_set = true;
}
void SetTime_Entry(DozClock *ctx)
{
    ctx->digit_sel = 0;
    ctx->user_time_ms = 0;
    Display_SetTime();
}
void SetTime_Update(DozClock *ctx)
{
    if (curr_format == TRAD_24H || curr_format == TRAD_12H) {
        ctx->user_time_ms = (uint32_t) (10*ctx->digit_vals[4] + ctx->digit_vals[5]) * 1000 +
                             (uint32_t) (10*ctx->digit_vals[2] + ctx->digit_vals[3]) * 60000;
        if (curr_format == TRAD_24H) {
            ctx->user_time_ms += (uint32_t) (10*ctx->digit_vals[0] + ctx->digit_vals[1]) * 3600000;
        } else { // TRAD_12H
            if (ctx->digit_vals[6] == 1 && (10*ctx->digit_vals[0] + ctx->digit_vals[1]) != 12) // PM && hour != 12
                ctx->user_time_ms += (uint32_t) (10*ctx->digit_vals[0] + ctx->digit_vals[1] + 12) * 3600000;
            else
                ctx->user_time_ms += (uint32_t) (10*ctx->digit_vals[0] + ctx->digit_vals[1]) * 3600000;
        }
    } else if (curr_format == DOZ_SEMI) {
        ctx->user_time_ms = (uint32_t) ctx->digit_vals[0] * 43200000 +
                             (uint32_t) ctx->digit_vals[1] * 3600000 +
                             (uint32_t) ctx->digit_vals[2] * 300000 +
                             (uint32_t) ctx->digit_vals[3] * 25000 +
                             (uint32_t) (ctx->digit_vals[4] * 25000) / 12;
    } else {
        ctx->user_time_ms = (uint32_t) ctx->digit_vals[0] * 7200000 +
                             (uint32_t) ctx->digit_vals[1] * 600000 +
                             (uint32_t) ctx->digit_vals[2] * 50000 +
                             (uint32_t) (ctx->digit_vals[3] * 25000) / 6;
        if (curr_format == DOZ_DRN5)
            ctx->user_time_ms += (uint32_t) (ctx->digit_vals[4] * 25000) / 72;
    } 
}
void SetTime_Exit(DozClock *ctx)
{
    if (cancel_pressed) {
        cancel_pressed = 0;
        return;
    }

    RtcTime time;
    msToRtcTime(ctx->user_time_ms, &time);
    Rtc_SetTime(&time);
    if (TimeTrack_SyncToRtc() != CLOCK_OK)
    {
        ctx->error_code = TIME_INIT;
        ctx->error_handler();
    }
}
static void AlarmTimerDispOn_Entry(DozClock *ctx)
{
    if (ctx->alarm_triggered) {
        // Display alarm triggered on display
    } else if (ctx->timer_triggered) {
        // Display timer triggered on display
    }
    Buzzer_Start();
}
static void AlarmTimerDispOn_Update(DozClock *ctx)
{
    UNUSED(ctx);
    // Countdown
    // Display time left on alarm trigger?
}
static void AlarmTimerDispOn_Exit(DozClock *ctx)
{
    UNUSED(ctx);
    Buzzer_Stop();
}
static void AlarmTimerDispOff_Entry(DozClock *ctx)
{
    Buzzer_Start();
}
static void AlarmTimerDispOff_Update(DozClock *ctx)
{
    UNUSED(ctx);
    // Countdown
}
static void AlarmTimerDispOff_Exit(DozClock *ctx)
{
    Buzzer_Stop();
    if (ctx->alarm_triggered) {
        // Stop displaying alarm triggered on display
        ctx->alarm_triggered = false;
        ctx->alarm_set = false;
    } else if (ctx->timer_triggered) {
        // Stop displaying timer triggered on display
        ctx->timer_triggered = false;
        ctx->timer_set = false;
    }
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
    if (state_event_map[g_clock_fsm.curr_state->state_code][g_clock_fsm.ctx->curr_event] != NULL) 
    {
        (*state_event_map[g_clock_fsm.curr_state->state_code][g_clock_fsm.ctx->curr_event]) ();
    }
    g_clock_fsm.ctx->curr_event = E_NONE;
}


// STATE EVENT MAP

static void toggle_mode(void);
static void toggle_doz_mode(void);
static void toggle_trad_mode(void);
static void toggle_alarm_set(void);
static void toggle_timer_set(void);
static void toggle_display(void);
static void transition_idle_disp_off(void);
static void transition_idle_disp_on(void);
static void transition_set_alarm(void);
static void transition_set_timer(void);
static void transition_set_time_doz(void);
static void transition_set_time_trad(void);
static void set_low_brightness(void);
static void set_high_brightness(void);
static void set_state_right_short(void);
static void set_state_left_short(void);
static void set_state_up_short(void);
static void set_state_down_short(void);
static void set_am_pm(void);
static void rtc_demo_reset(void);
static void cancel_set_state(void);
static void vol_up_short(void);
static void vol_up_long(void);
static void vol_down_short(void);
static void vol_down_long(void);

static void transition_alarm_disp_on(void);
static void transition_timer_disp_on(void);
static void transition_alarm_disp_off(void);
static void transition_timer_disp_off(void);
static void set_time_doz_done(void);
static void set_time_trad_done(void);

static void state_event_map_init() 
{
    // IDLE ON
    state_event_map[STATE_IDLE_DISP_ON][E_DISPLAY_SHORT]            = toggle_mode;
    state_event_map[STATE_IDLE_DISP_ON][E_DISPLAY_LONG]             = transition_idle_disp_off;
    state_event_map[STATE_IDLE_DISP_ON][E_DOZ_SHORT]                = toggle_doz_mode;
    state_event_map[STATE_IDLE_DISP_ON][E_DOZ_LONG]                 = transition_set_time_doz;
    state_event_map[STATE_IDLE_DISP_ON][E_TRAD_SHORT]               = toggle_trad_mode;
    state_event_map[STATE_IDLE_DISP_ON][E_TRAD_LONG]                = transition_set_time_trad;
    state_event_map[STATE_IDLE_DISP_ON][E_ALARM_SHORT]              = toggle_alarm_set;
    state_event_map[STATE_IDLE_DISP_ON][E_ALARM_LONG]               = transition_set_alarm;
    state_event_map[STATE_IDLE_DISP_ON][E_TIMER_SHORT]              = toggle_timer_set;
    state_event_map[STATE_IDLE_DISP_ON][E_TIMER_LONG]               = transition_set_timer;
    state_event_map[STATE_IDLE_DISP_ON][E_ROOM_DARK]                = set_low_brightness;
    state_event_map[STATE_IDLE_DISP_ON][E_ROOM_LIGHT]               = set_high_brightness;
    state_event_map[STATE_IDLE_DISP_ON][E_CANCEL_LONG]              = rtc_demo_reset;
    state_event_map[STATE_IDLE_DISP_ON][E_VOLUP_SHORT]              = vol_up_short;
    state_event_map[STATE_IDLE_DISP_ON][E_VOLUP_LONG]               = vol_up_long;
    state_event_map[STATE_IDLE_DISP_ON][E_VOLDOWN_SHORT]            = vol_down_short;
    state_event_map[STATE_IDLE_DISP_ON][E_VOLDOWN_LONG]             = vol_down_long;
    state_event_map[STATE_IDLE_DISP_ON][E_ALARM_TRIG]               = transition_alarm_disp_on;
    state_event_map[STATE_IDLE_DISP_ON][E_TIMER_TRIG]               = transition_timer_disp_on;

    // IDLE OFF
    state_event_map[STATE_IDLE_DISP_OFF][E_DISPLAY_SHORT]           = toggle_mode;
    state_event_map[STATE_IDLE_DISP_OFF][E_DISPLAY_LONG]            = transition_idle_disp_on;
    state_event_map[STATE_IDLE_DISP_OFF][E_ROOM_DARK]               = set_low_brightness;
    state_event_map[STATE_IDLE_DISP_OFF][E_ROOM_LIGHT]              = set_high_brightness;
    state_event_map[STATE_IDLE_DISP_OFF][E_VOLUP_SHORT]             = vol_up_short;
    state_event_map[STATE_IDLE_DISP_OFF][E_VOLUP_LONG]              = vol_up_long;
    state_event_map[STATE_IDLE_DISP_OFF][E_VOLDOWN_SHORT]           = vol_down_short;
    state_event_map[STATE_IDLE_DISP_OFF][E_VOLDOWN_LONG]            = vol_down_long;
    state_event_map[STATE_IDLE_DISP_OFF][E_ALARM_TRIG]              = transition_alarm_disp_off;
    state_event_map[STATE_IDLE_DISP_OFF][E_TIMER_TRIG]              = transition_timer_disp_off;

    // SET ALARM
    state_event_map[STATE_SET_ALARM][E_DISPLAY_SHORT]               = toggle_mode;
    state_event_map[STATE_SET_ALARM][E_DISPLAY_LONG]                = toggle_display;
    state_event_map[STATE_SET_ALARM][E_DOZ_SHORT]                   = toggle_doz_mode;
    state_event_map[STATE_SET_ALARM][E_TRAD_SHORT]                  = toggle_trad_mode;
    state_event_map[STATE_SET_ALARM][E_TRAD_LONG]                   = set_am_pm;
    state_event_map[STATE_SET_ALARM][E_LEFT_SHORT]                  = set_state_left_short;
    state_event_map[STATE_SET_ALARM][E_RIGHT_SHORT]                 = set_state_right_short;
    state_event_map[STATE_SET_ALARM][E_UP_SHORT]                    = set_state_up_short;
    state_event_map[STATE_SET_ALARM][E_DOWN_SHORT]                  = set_state_down_short;
    state_event_map[STATE_SET_ALARM][E_CANCEL_SHORT]                = cancel_set_state;
    state_event_map[STATE_SET_ALARM][E_VOLUP_SHORT]                 = vol_up_short;
    state_event_map[STATE_SET_ALARM][E_VOLUP_LONG]                  = vol_up_long;
    state_event_map[STATE_SET_ALARM][E_VOLDOWN_SHORT]               = vol_down_short;
    state_event_map[STATE_SET_ALARM][E_VOLDOWN_LONG]                = vol_down_long;
    state_event_map[STATE_SET_ALARM][E_ALARM_LONG]                  = transition_idle_disp_on;

    // SET TIMER
    state_event_map[STATE_SET_TIMER][E_DISPLAY_SHORT]               = toggle_mode;
    state_event_map[STATE_SET_TIMER][E_DISPLAY_LONG]                = toggle_display;
    state_event_map[STATE_SET_TIMER][E_DOZ_SHORT]                   = toggle_doz_mode;
    state_event_map[STATE_SET_TIMER][E_TRAD_SHORT]                  = toggle_trad_mode;
    state_event_map[STATE_SET_TIMER][E_LEFT_SHORT]                  = set_state_left_short;
    state_event_map[STATE_SET_TIMER][E_RIGHT_SHORT]                 = set_state_right_short;
    state_event_map[STATE_SET_TIMER][E_UP_SHORT]                    = set_state_up_short;
    state_event_map[STATE_SET_TIMER][E_DOWN_SHORT]                  = set_state_down_short;
    state_event_map[STATE_SET_TIMER][E_CANCEL_SHORT]                = cancel_set_state;
    state_event_map[STATE_SET_TIMER][E_VOLUP_SHORT]                 = vol_up_short;
    state_event_map[STATE_SET_TIMER][E_VOLUP_LONG]                  = vol_up_long;
    state_event_map[STATE_SET_TIMER][E_VOLDOWN_SHORT]               = vol_down_short;
    state_event_map[STATE_SET_TIMER][E_VOLDOWN_LONG]                = vol_down_long;
    state_event_map[STATE_SET_TIMER][E_TIMER_LONG]                  = transition_idle_disp_on;

    // SET TIME
    state_event_map[STATE_SET_TIME][E_DISPLAY_SHORT]                = toggle_mode;
    state_event_map[STATE_SET_TIME][E_DISPLAY_LONG]                 = toggle_display;
    state_event_map[STATE_SET_TIME][E_DOZ_SHORT]                    = toggle_doz_mode;
    state_event_map[STATE_SET_TIME][E_DOZ_LONG]                     = set_time_doz_done;
    state_event_map[STATE_SET_TIME][E_TRAD_SHORT]                   = toggle_trad_mode;
    state_event_map[STATE_SET_TIME][E_TRAD_LONG]                    = set_time_trad_done;
    state_event_map[STATE_SET_TIME][E_LEFT_SHORT]                   = set_state_left_short;
    state_event_map[STATE_SET_TIME][E_RIGHT_SHORT]                  = set_state_right_short;
    state_event_map[STATE_SET_TIME][E_UP_SHORT]                     = set_state_up_short;
    state_event_map[STATE_SET_TIME][E_DOWN_SHORT]                   = set_state_down_short;
    state_event_map[STATE_SET_TIME][E_CANCEL_SHORT]                 = cancel_set_state;
    state_event_map[STATE_SET_TIME][E_VOLUP_SHORT]                  = vol_up_short;
    state_event_map[STATE_SET_TIME][E_VOLUP_LONG]                   = vol_up_long;
    state_event_map[STATE_SET_TIME][E_VOLDOWN_SHORT]                = vol_down_short;
    state_event_map[STATE_SET_TIME][E_VOLDOWN_LONG]                 = vol_down_long;

    // ALARM TIMER TRIGGERED DISP ON
    state_event_map[STATE_ALARM_TIMER_DISP_ON][E_CANCEL_SHORT]      = transition_idle_disp_on;

    // ALARM TIMER TRIGGERED DISP OFF
    state_event_map[STATE_ALARM_TIMER_DISP_OFF][E_CANCEL_SHORT]     = transition_idle_disp_on;
}

static void set_time_doz_done(void)
{
    curr_format = doz_format_list[doz_index];
    Display_SetFormat(curr_format);
    transition(&s_idle_disp_on);
}
static void set_time_trad_done(void)
{
    curr_format = trad_format_list[trad_index];
    Display_SetFormat(curr_format);
    transition(&s_idle_disp_on);
}
static void transition_alarm_disp_on(void)
{
    g_clock_fsm.ctx->alarm_triggered = true;
    transition(&s_alarm_timer_disp_on);
}
static void transition_timer_disp_on(void)
{
    g_clock_fsm.ctx->timer_triggered = true;
    transition(&s_alarm_timer_disp_on);
}
static void transition_alarm_disp_off(void)
{
    g_clock_fsm.ctx->alarm_triggered = true;
    transition(&s_alarm_timer_disp_off);
}
static void transition_timer_disp_off(void)
{
    g_clock_fsm.ctx->timer_triggered = true;
    transition(&s_alarm_timer_disp_off);
}

static void set_state_right_short(void)
{
    if (curr_format == TRAD_24H || curr_format == TRAD_12H)
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 1) % 6;
    else if (curr_format == DOZ_DRN5 || curr_format == DOZ_SEMI)
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 1) % 5;
    else
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 1) % 4;
}
static void set_state_left_short(void)
{
    if (curr_format == TRAD_24H || curr_format == TRAD_12H)
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 6 - 1) % 6;
    else if (curr_format == DOZ_DRN5 || curr_format == DOZ_SEMI)
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 5 - 1) % 5;
    else
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 4 - 1) % 4;
}
static void set_state_up_short(void)
{
    if (curr_format == TRAD_24H || curr_format == TRAD_12H)
        g_clock_fsm.ctx->digit_vals[g_clock_fsm.ctx->digit_sel] = (g_clock_fsm.ctx->digit_vals[g_clock_fsm.ctx->digit_sel] + 1) % 9;
    else
        g_clock_fsm.ctx->digit_vals[g_clock_fsm.ctx->digit_sel] = (g_clock_fsm.ctx->digit_vals[g_clock_fsm.ctx->digit_sel] + 1) % 11;
}
static void set_state_down_short(void)
{
    if (curr_format == TRAD_24H || curr_format == TRAD_12H)
        g_clock_fsm.ctx->digit_vals[g_clock_fsm.ctx->digit_sel] = (g_clock_fsm.ctx->digit_vals[g_clock_fsm.ctx->digit_sel] + 9 - 1) % 9;
    else
        g_clock_fsm.ctx->digit_vals[g_clock_fsm.ctx->digit_sel] = (g_clock_fsm.ctx->digit_vals[g_clock_fsm.ctx->digit_sel] + 11 - 1) % 11;
}
static void set_low_brightness(void)
{
    Display_SetBrightness(LOW_BRIGHTNESS);
}
static void set_high_brightness(void)
{
    Display_SetBrightness(HIGH_BRIGHTNESS);
}
static void set_am_pm(void)
{
    if (curr_format == TRAD_12H)
        g_clock_fsm.ctx->digit_vals[6] = !g_clock_fsm.ctx->digit_vals[6];
}
static void transition_set_alarm(void)
{ 
    transition(&s_set_alarm);
}
static void transition_set_timer(void)
{
    transition(&s_set_timer);
}
static void transition_idle_disp_on(void) 
{ 
    transition(&s_idle_disp_on);
}
static void transition_idle_disp_off(void) 
{
    transition(&s_idle_disp_off);
}
static void transition_set_time_doz(void)
{
    curr_format = doz_format_list[doz_index];
    Display_SetFormat(curr_format);
    transition(&s_set_time);
}
static void transition_set_time_trad(void)
{
    curr_format = trad_format_list[trad_index];
    Display_SetFormat(curr_format);
    transition(&s_set_time);
}
static void toggle_display(void)
{
    if (displayOn) {
        displayOn = 0;
        Display_Off();
    } else {
        displayOn = 1;
        Display_On();
    }
}
static void toggle_alarm_set(void)
{
    g_clock_fsm.ctx->alarm_set = !g_clock_fsm.ctx->alarm_set;
    Rtc_EnableAlarm(ALARM, g_clock_fsm.ctx->alarm_set);
}
static void toggle_timer_set(void)
{
    g_clock_fsm.ctx->timer_set = !g_clock_fsm.ctx->timer_set;
    Rtc_EnableAlarm(TIMER, g_clock_fsm.ctx->timer_set);
}
static void toggle_trad_mode(void) 
{ 
    trad_index = (trad_index + 1) % 2;
    curr_format = trad_format_list[trad_index];
    Display_SetFormat(curr_format);
}
static void toggle_doz_mode(void) 
{ 
    doz_index = (doz_index + 1) % 3;
    curr_format = doz_format_list[doz_index];
    Display_SetFormat(curr_format);
}
static void toggle_mode(void)
{
    Display_ToggleMode();
}
static void rtc_demo_reset(void)
{
    Rtc_SetTime(&demo_reset);
}
static void cancel_set_state(void)
{
    cancel_pressed = 1;
    transition(&s_idle_disp_off);
}
static void vol_up_short(void)
{
    if (Buzzer_SetVolume(g_clock_fsm.ctx->buzzer->volume + 1) == CLOCK_OK) {
        g_clock_fsm.ctx->buzzer->volume += 1;
    }
}
static void vol_up_long(void)
{
    Buzzer_SetVolumeMax();
    g_clock_fsm.ctx->buzzer->volume = MAX_VOLUME;
}
static void vol_down_short(void)
{
    if (Buzzer_SetVolume(g_clock_fsm.ctx->buzzer->volume - 1) == CLOCK_OK) {
        g_clock_fsm.ctx->buzzer->volume -= 1;
    }
}
static void vol_down_long(void)
{
    Buzzer_SetVolumeMin();
    g_clock_fsm.ctx->buzzer->volume = MIN_VOLUME;
}

#ifdef NO_PLATFORM
int main(void)
{
    return 0;
}
#endif

