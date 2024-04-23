#include "doz_clock.h"

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

static void transition(DozClockState *next);
static void process_event();
static void state_event_map_init();
static void update_user_timer(DozClock *ctx, uint32_t time_elapsed);
static void transition_digits(TimeFormats timeFormat, uint32_t ms, uint8_t *vals);

static void (*state_event_map[NUM_STATES][NUM_EVENTS]) (void);

static ExternVars clock_vars = { 0 };
static DozClockFSM g_clock_fsm = { 0 };
static uint8_t timer_delay = 0;

static TimeFormats trad_format_list[] = {TRAD_24H, TRAD_12H};
static TimeFormats doz_format_list[] = {DOZ_SEMI, DOZ_DRN4, DOZ_DRN5};
static TimeFormats curr_format;

static uint8_t trad_index = 0, doz_index = 1;
static uint8_t cancel_pressed = 0;
static uint32_t user_alarm_ms_old, user_timer_ms_old, time_ms_old;
static uint8_t alarm_set_old, timer_set_old;
static uint32_t timer_end_ms, curr_set_timer_ms = TIME_24H_MS;
static uint32_t buzzer_countdown_ms;

static uint32_t increments;

static RtcTime demo_reset = {
        .hr = 17,
        .min = 22,
        .sec = 0
};

static RtcTime alarm_time;

static uint32_t curr_timer_ms, curr_time_ms, curr_alarm_ms;
static bool digits_changed = false;

// State definitions
static DozClockState s_init =
{
    .state_code = STATE_INIT,
    .entry      = Init_Entry,
    .update     = Init_Update,
    .exit       = Init_Exit,
};
static DozClockState s_idle_disp_on =
{
    .state_code = STATE_IDLE_DISP_ON,
    .entry      = IdleDispOn_Entry,
    .update     = IdleDispOn_Update,
    .exit       = Default_Exit,
};
static DozClockState s_idle_disp_off =
{
    .state_code = STATE_IDLE_DISP_OFF,
    .entry      = IdleDispOff_Entry,
    .update     = IdleDispOff_Update,
    .exit       = IdleDispOff_Exit,
};
static DozClockState s_set_alarm =
{
    .state_code = STATE_SET_ALARM,
    .entry      = SetAlarm_Entry,
    .update     = SetAlarm_Update,
    .exit       = SetAlarm_Exit,
};
static DozClockState s_set_timer =
{
    .state_code = STATE_SET_TIMER,
    .entry      = SetTimer_Entry,
    .update     = SetTimer_Update,
    .exit       = SetTimer_Exit,
};
static DozClockState s_set_time =
{
    .state_code = STATE_SET_TIME,
    .entry      = SetTime_Entry,
    .update     = SetTime_Update,
    .exit       = SetTime_Exit,
};
static DozClockState s_alarm_timer_disp_on =
{
    .state_code = STATE_ALARM_TIMER_DISP_ON,
    .entry      = AlarmTimerDispOn_Entry,
    .update     = AlarmTimerDispOn_Update,
    .exit       = AlarmTimerDispOn_Exit,
};
static DozClockState s_alarm_timer_disp_off =
{
    .state_code = STATE_ALARM_TIMER_DISP_OFF,
    .entry      = AlarmTimerDispOff_Entry,
    .update     = AlarmTimerDispOff_Update,
    .exit       = AlarmTimerDispOff_Exit,
};

// FSM Event Functions
void DozClock_Init(DozClock *ctx)
{
    clock_vars.alarm_set                = &ctx->alarm_set;
    clock_vars.alarm_triggered          = &ctx->alarm_triggered;
    clock_vars.timer_set                = &ctx->timer_set;
    clock_vars.timer_triggered          = &ctx->timer_triggered;
    clock_vars.show_error               = &ctx->show_error;
    clock_vars.digit_sel                = &ctx->digit_sel;
    clock_vars.digit_vals               = ctx->digit_vals;
    clock_vars.error_code               = &ctx->error_code;
    clock_vars.time_ms                  = &ctx->time_ms;
    clock_vars.user_alarm_ms            = &ctx->user_alarm_ms;
    clock_vars.user_time_ms             = &ctx->user_time_ms;
    clock_vars.user_timer_ms            = &ctx->user_timer_ms;
    clock_vars.timer_alarm_displayed    = &ctx->timer_alarm_displayed;
    clock_vars.diurn_radix_pos          = &ctx->diurn_radix_pos;
    clock_vars.semi_diurn_radix_pos     = &ctx->semi_diurn_radix_pos;

    *clock_vars.alarm_set               = 0;
    *clock_vars.alarm_triggered         = 0;
    *clock_vars.timer_set               = 0;
    *clock_vars.timer_triggered         = 0;
    *clock_vars.show_error              = 0;
    *clock_vars.digit_sel               = 0;
    clock_vars.digit_vals[0]            = 0;
    clock_vars.digit_vals[1]            = 0;
    clock_vars.digit_vals[2]            = 0;
    clock_vars.digit_vals[3]            = 0;
    clock_vars.digit_vals[4]            = 0;
    clock_vars.digit_vals[5]            = 0;
    clock_vars.digit_vals[6]            = 0;
    *clock_vars.error_code              = 0;
    *clock_vars.time_ms                 = 0;
    *clock_vars.user_alarm_ms           = 0;
    *clock_vars.user_time_ms            = 0;
    *clock_vars.user_timer_ms           = TIME_24H_MS;
    *clock_vars.timer_alarm_displayed   = DISPLAY_TIMER;
    *clock_vars.diurn_radix_pos         = RADIX_POS3;
    *clock_vars.semi_diurn_radix_pos    = RADIX_POS2;

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
    if (g_clock_fsm.ctx->timer_set) 
    {
        update_user_timer(g_clock_fsm.ctx, TIMER_PERIOD_MS);
    }
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
    Display_SetFormat(DOZ_DRN4);
    curr_format = DOZ_DRN4;

    if (TimeTrack_SyncToRtc() != CLOCK_OK)
    {
        ctx->error_code = TIME_INIT;
        ctx->error_handler();
    }
    TimeTrack_GetTimeMs(&ctx->time_ms);

    Rtc_GetAlarmStatus(ALARM, &ctx->alarm_set);
    if (ctx->alarm_set)
    {
        Rtc_GetAlarm(&alarm_time, ALARM);
        ctx->user_alarm_ms = (uint32_t) (alarm_time.sec) * 1000 +   // sec
                             (uint32_t) (alarm_time.min) * 60000 +  // min
                             (uint32_t) (alarm_time.hr) * 3600000;  // hour
    }

    transition(&s_idle_disp_on);
}
void Init_Exit(DozClock *ctx)
{
    TimeTrack_GetTimeMs(&ctx->time_ms);
    Display_On();
}
void IdleDispOn_Entry(DozClock *ctx)
{
    UNUSED(ctx);
    Display_On();
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
}
void SetAlarm_Entry(DozClock *ctx)
{
    user_alarm_ms_old = ctx->user_alarm_ms;
    alarm_set_old = ctx->alarm_set;
    ctx->digit_sel = 0;
    ctx->alarm_set = false;
    curr_alarm_ms = ctx->user_alarm_ms;
    digits_changed = false;

    transition_digits(curr_format, ctx->user_alarm_ms, ctx->digit_vals);
    Rtc_EnableAlarm(ALARM, ctx->alarm_set);
    Display_SetAlarm();

    // Disable timer
    timer_set_old = ctx->timer_set;
    ctx->timer_set = false;
    Rtc_EnableAlarm(TIMER, ctx->timer_set);
}
void SetAlarm_Update(DozClock *ctx)
{
    TimeTrack_GetTimeMs(&ctx->time_ms);

    if (!digits_changed) {
        ctx->user_alarm_ms = curr_alarm_ms;
    } else {
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

            increments = ctx->digit_vals[0]*pow(12,4) 
                            + ctx->digit_vals[1]*pow(12,3)
                            + ctx->digit_vals[2]*pow(12,2)
                            + ctx->digit_vals[3]*pow(12,1)
                            + ctx->digit_vals[4];

            ctx->user_alarm_ms = round(increments*2083.33333333);

        } else { // DOZ_DRN4 || DOZ_DRN5

            increments = ctx->digit_vals[0]*pow(12,4) 
                            + ctx->digit_vals[1]*pow(12,3)
                            + ctx->digit_vals[2]*pow(12,2)
                            + ctx->digit_vals[3]*pow(12,1)
                            + ((curr_format == DOZ_DRN5) ? ctx->digit_vals[4] : 0);

            ctx->user_alarm_ms = round(increments*347.22222222);

        }

        curr_alarm_ms = ctx->user_alarm_ms;
    }
}
void SetAlarm_Exit(DozClock *ctx)
{
    ctx->timer_set = timer_set_old;
    Rtc_EnableAlarm(TIMER, ctx->timer_set);

    if (cancel_pressed) {
        cancel_pressed = 0;
        ctx->user_alarm_ms = user_alarm_ms_old;
        ctx->alarm_set = alarm_set_old;
        Rtc_EnableAlarm(ALARM, ctx->alarm_set);
        return;
    }

    RtcTime alarmTime;
    ctx->alarm_set = true;

    msToRtcTime(ctx->user_alarm_ms, &alarmTime);
    Rtc_SetAlarm(&alarmTime, ALARM);
    Rtc_EnableAlarm(ALARM, ctx->alarm_set);

    g_clock_fsm.ctx->timer_alarm_displayed = DISPLAY_ALARM;
}
void SetTimer_Entry(DozClock *ctx)
{
    if (curr_format == TRAD_12H) {
        curr_format = TRAD_24H;
    } else if (curr_format == DOZ_DRN4 || curr_format == DOZ_SEMI) {
        curr_format = DOZ_DRN5;
    }
    Display_SetFormat(curr_format);

    user_timer_ms_old = ctx->user_timer_ms;
    timer_set_old = ctx->timer_set;
    ctx->digit_sel = 0;
    for (unsigned i = 0; i < MAX_DIGITS ;++i)
        ctx->digit_vals[i] = 0;
    ctx->user_timer_ms = 0;
    ctx->timer_set = false;
    curr_timer_ms = 0;
    digits_changed = false;

    Rtc_EnableAlarm(TIMER, ctx->timer_set);
    Display_SetTimer();

    // Disable alarm
    alarm_set_old = ctx->alarm_set;
    ctx->alarm_set = false;
    Rtc_EnableAlarm(ALARM, ctx->alarm_set);
}
void SetTimer_Update(DozClock *ctx)
{
    TimeTrack_GetTimeMs(&ctx->time_ms);

    if (!digits_changed) {
        ctx->user_timer_ms = curr_timer_ms;
    } else {
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

            increments = ctx->digit_vals[0]*pow(12,4) 
                            + ctx->digit_vals[1]*pow(12,3)
                            + ctx->digit_vals[2]*pow(12,2)
                            + ctx->digit_vals[3]*pow(12,1)
                            + ctx->digit_vals[4];

            ctx->user_timer_ms = round(increments*2083.33333333);

        } else { // DOZ_DRN4 || DOZ_DRN5

            increments = ctx->digit_vals[0]*pow(12,4) 
                            + ctx->digit_vals[1]*pow(12,3)
                            + ctx->digit_vals[2]*pow(12,2)
                            + ctx->digit_vals[3]*pow(12,1)
                            + ((curr_format == DOZ_DRN5) ? ctx->digit_vals[4] : 0);

            ctx->user_timer_ms = round(increments*347.22222222);

        }
        
        curr_timer_ms = ctx->user_timer_ms;
    }
}
void SetTimer_Exit(DozClock *ctx)
{
    ctx->alarm_set = alarm_set_old;
    Rtc_EnableAlarm(ALARM, ctx->alarm_set);

    RtcTime timerTime;

    if (cancel_pressed) {
        cancel_pressed = 0;
        ctx->user_timer_ms = user_timer_ms_old;
        ctx->timer_set = timer_set_old;
    } else {
        ctx->timer_set = true;
        g_clock_fsm.ctx->timer_alarm_displayed = DISPLAY_TIMER;
    }

    curr_set_timer_ms = ctx->user_timer_ms;
    timer_end_ms = (curr_set_timer_ms + ctx->time_ms) % TIME_24H_MS;

    msToRtcTime(timer_end_ms, &timerTime);
    Rtc_SetAlarm(&timerTime, TIMER);
    Rtc_EnableAlarm(TIMER, ctx->timer_set);
}
void SetTime_Entry(DozClock *ctx)
{
    ctx->digit_sel = 0;
    ctx->user_time_ms = ctx->time_ms;
    curr_time_ms = ctx->time_ms;
    digits_changed = false;

    transition_digits(curr_format, ctx->user_time_ms, ctx->digit_vals);
    Display_SetTime();

    // Disable alarm and timer
    alarm_set_old = ctx->alarm_set;
    timer_set_old = ctx->timer_set;
    ctx->alarm_set = false;
    ctx->timer_set = false;
    Rtc_EnableAlarm(ALARM, ctx->alarm_set);
    Rtc_EnableAlarm(TIMER, ctx->timer_set);
}
void SetTime_Update(DozClock *ctx)
{
    TimeTrack_GetTimeMs(&ctx->time_ms);

    if (!digits_changed) {
        ctx->user_time_ms = curr_time_ms;
    } else {
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

            increments = ctx->digit_vals[0]*pow(12,4) 
                            + ctx->digit_vals[1]*pow(12,3)
                            + ctx->digit_vals[2]*pow(12,2)
                            + ctx->digit_vals[3]*pow(12,1)
                            + ctx->digit_vals[4];

            ctx->user_time_ms = round(increments*2083.33333333);

        } else { // DOZ_DRN4 || DOZ_DRN5

            increments = ctx->digit_vals[0]*pow(12,4) 
                            + ctx->digit_vals[1]*pow(12,3)
                            + ctx->digit_vals[2]*pow(12,2)
                            + ctx->digit_vals[3]*pow(12,1)
                            + ((curr_format == DOZ_DRN5) ? ctx->digit_vals[4] : 0);

            ctx->user_time_ms = round(increments*347.22222222);

        }

        curr_time_ms = ctx->user_time_ms;
    }
}
void SetTime_Exit(DozClock *ctx)
{
    ctx->alarm_set = alarm_set_old;
    ctx->timer_set = timer_set_old;
    Rtc_EnableAlarm(ALARM, ctx->alarm_set);
    Rtc_EnableAlarm(TIMER, ctx->timer_set);

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
        ctx->alarm_triggered = false;
        ctx->alarm_set = false;

        if (ctx->timer_alarm_displayed == DISPLAY_ALARM && ctx->timer_set)
            ctx->timer_alarm_displayed = DISPLAY_TIMER;
    } else if (ctx->timer_triggered) {
        ctx->timer_triggered = false;
        ctx->timer_set = false;
        ctx->user_timer_ms = curr_set_timer_ms;

        if (ctx->timer_alarm_displayed == DISPLAY_TIMER && ctx->alarm_set)
            ctx->timer_alarm_displayed = DISPLAY_ALARM;
    }

    buzzer_countdown_ms = 0;
    time_ms_old = ctx->time_ms;
    Buzzer_Start();
}
static void AlarmTimerDispOn_Update(DozClock *ctx)
{
    TimeTrack_GetTimeMs(&ctx->time_ms);

    if (ctx->time_ms >= time_ms_old)
        buzzer_countdown_ms += (ctx->time_ms - time_ms_old);
    else
        buzzer_countdown_ms += ctx->time_ms;
    time_ms_old = ctx->time_ms;

    if (buzzer_countdown_ms >= 30000) // Buzzer turns off after 30 seconds
        transition(&s_idle_disp_on);

}
static void AlarmTimerDispOn_Exit(DozClock *ctx)
{
    UNUSED(ctx);
    Buzzer_Stop();
}
static void AlarmTimerDispOff_Entry(DozClock *ctx)
{
    if (ctx->alarm_triggered) {
        ctx->alarm_triggered = false;
        ctx->alarm_set = false;

        if (ctx->timer_alarm_displayed == DISPLAY_ALARM && ctx->timer_set)
            ctx->timer_alarm_displayed = DISPLAY_TIMER;
    } else if (ctx->timer_triggered) {
        ctx->timer_triggered = false;
        ctx->timer_set = false;
        ctx->user_timer_ms = curr_set_timer_ms;

        if (ctx->timer_alarm_displayed == DISPLAY_TIMER && ctx->alarm_set)
            ctx->timer_alarm_displayed = DISPLAY_ALARM;
    }

    buzzer_countdown_ms = 0;
    time_ms_old = ctx->time_ms;
    Buzzer_Start();
}
static void AlarmTimerDispOff_Update(DozClock *ctx)
{
    TimeTrack_GetTimeMs(&ctx->time_ms);

    if (ctx->time_ms >= time_ms_old)
        buzzer_countdown_ms += (ctx->time_ms - time_ms_old);
    else
        buzzer_countdown_ms += ctx->time_ms;
    time_ms_old = ctx->time_ms;

    if (buzzer_countdown_ms >= 30000) // Buzzer turns off after 30 seconds
        transition(&s_idle_disp_off);

}
static void AlarmTimerDispOff_Exit(DozClock *ctx)
{
    UNUSED(ctx);
    Buzzer_Stop();
}

// Helper functions
static void transition(DozClockState *next)
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

static void update_user_timer(DozClock *ctx, uint32_t time_elapsed)
{
    ctx->user_timer_ms -= time_elapsed;
}

static void transition_digits(TimeFormats timeFormat, uint32_t ms, uint8_t *vals)
{
    if (timeFormat == TRAD_24H || timeFormat == TRAD_12H)
    {
        uint8_t hr, min, sec;
        
        msToTrad(ms, &hr, &min, &sec);

        if (timeFormat == TRAD_12H && hr > 12) {
            hr -= 12;
            vals[6] = 1;
        } else {
            if (hr == 12)
                vals[6] = 1;
            else
                vals[6] = 0;
        }
        vals[0] = hr / 10;
        vals[1] = hr % 10;
        vals[2] = min / 10;
        vals[3] = min % 10;
        vals[4] = sec / 10;
        vals[5] = sec % 10;
    }
    else
    {
        uint8_t digit1, digit2, digit3, digit4, digit5;

        if (timeFormat == DOZ_DRN4 || timeFormat == DOZ_DRN5)
            msToDiurn(ms, &digit1, &digit2, &digit3, &digit4, &digit5);
        else if (timeFormat == DOZ_SEMI)
            msToSemiDiurn(ms, &digit1, &digit2, &digit3, &digit4, &digit5);

        vals[0] = digit1;
        vals[1] = digit2;
        vals[2] = digit3;
        vals[3] = digit4;
        vals[4] = digit5;
    }
}

void msToTrad(uint32_t time_ms, uint8_t *hr_24, uint8_t *min, uint8_t *sec)
{
    time_ms = time_ms / 1000;
    *sec = time_ms % 60;
    time_ms = time_ms / 60;
    *min = time_ms % 60;
    time_ms = time_ms / 60;
    *hr_24 = time_ms % 24;
}

void msToDiurn(uint32_t time_ms, uint8_t *digit1, uint8_t *digit2, uint8_t *digit3, uint8_t *digit4, uint8_t *digit5)
{
    increments = (uint32_t) round(time_ms / 347.22222222);

    *digit5 = increments % 12;
    increments /= 12;
    *digit4 = increments % 12;
    increments /= 12;
    *digit3 = increments % 12;
    increments /= 12;
    *digit2 = increments % 12;
    increments /= 12;
    *digit1 = increments % 12;
}

void msToSemiDiurn(uint32_t time_ms, uint8_t *digit1, uint8_t *digit2, uint8_t *digit3, uint8_t *digit4, uint8_t *digit5)
{
    increments = (uint32_t) round(time_ms / 2083.33333333);

    *digit5 = increments % 12;
    increments /= 12;
    *digit4 = increments % 12;
    increments /= 12;
    *digit3 = increments % 12;
    increments /= 12;
    *digit2 = increments % 12;
    increments /= 12;
    *digit1 = increments % 2;
}


// STATE EVENT MAP

// State event map functions
static void toggle_mode(void);
static void toggle_doz_mode(void);
static void toggle_trad_mode(void);
static void toggle_alarm_set(void);
static void toggle_timer_set(void);
static void toggle_doz_timer(void);
static void toggle_trad_timer(void);
static void toggle_timer_alarm_displayed(void);
static void transition_idle_disp_off(void);
static void transition_idle_disp_on(void);
static void transition_set_alarm(void);
static void transition_set_timer(void);
static void transition_set_time_doz(void);
static void transition_set_time_trad(void);
static void transition_alarm_disp_on(void);
static void transition_timer_disp_on(void);
static void transition_alarm_disp_off(void);
static void transition_timer_disp_off(void);
static void set_low_brightness(void);
static void set_high_brightness(void);
static void set_state_right_short(void);
static void set_state_left_short(void);
static void set_state_up_short(void);
static void set_state_down_short(void);
static void set_time_doz_done(void);
static void set_time_trad_done(void);
static void rtc_demo_reset(void);
static void cancel_set_state(void);
static void vol_up_short(void);
static void vol_up_long(void);
static void vol_down_short(void);
static void vol_down_long(void);
static void radix_pos_left(void);
static void radix_pos_right(void);

// Helper functions
static void digit_value_increase(TimeFormats timeFormat, uint8_t *val, uint8_t sel);
static void digit_value_decrease(TimeFormats timeFormat, uint8_t *val, uint8_t sel);

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
    // state_event_map[STATE_IDLE_DISP_ON][E_CANCEL_LONG]              = rtc_demo_reset;
    state_event_map[STATE_IDLE_DISP_ON][E_VOLUP_SHORT]              = vol_up_short;
    state_event_map[STATE_IDLE_DISP_ON][E_VOLUP_LONG]               = vol_up_long;
    state_event_map[STATE_IDLE_DISP_ON][E_VOLDOWN_SHORT]            = vol_down_short;
    state_event_map[STATE_IDLE_DISP_ON][E_VOLDOWN_LONG]             = vol_down_long;
    state_event_map[STATE_IDLE_DISP_ON][E_ALARM_TRIG]               = transition_alarm_disp_on;
    state_event_map[STATE_IDLE_DISP_ON][E_TIMER_TRIG]               = transition_timer_disp_on;
    state_event_map[STATE_IDLE_DISP_ON][E_LEFT_SHORT]               = radix_pos_left;
    state_event_map[STATE_IDLE_DISP_ON][E_RIGHT_SHORT]              = radix_pos_right;
    state_event_map[STATE_IDLE_DISP_ON][E_UP_SHORT]                 = toggle_timer_alarm_displayed;
    state_event_map[STATE_IDLE_DISP_ON][E_DOWN_SHORT]               = toggle_timer_alarm_displayed;

    // IDLE OFF
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
    state_event_map[STATE_SET_ALARM][E_DOZ_SHORT]                   = toggle_doz_mode;
    state_event_map[STATE_SET_ALARM][E_TRAD_SHORT]                  = toggle_trad_mode;
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
    state_event_map[STATE_SET_ALARM][E_ROOM_DARK]                   = set_low_brightness;
    state_event_map[STATE_SET_ALARM][E_ROOM_LIGHT]                  = set_high_brightness;

    // SET TIMER
    state_event_map[STATE_SET_TIMER][E_DOZ_SHORT]                   = toggle_doz_timer;
    state_event_map[STATE_SET_TIMER][E_TRAD_SHORT]                  = toggle_trad_timer;
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
    state_event_map[STATE_SET_TIMER][E_ROOM_DARK]                   = set_low_brightness;
    state_event_map[STATE_SET_TIMER][E_ROOM_LIGHT]                  = set_high_brightness;

    // SET TIME
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
    state_event_map[STATE_SET_TIME][E_ROOM_DARK]                    = set_low_brightness;
    state_event_map[STATE_SET_TIME][E_ROOM_LIGHT]                   = set_high_brightness;

    // ALARM TIMER TRIGGERED DISP ON
    state_event_map[STATE_ALARM_TIMER_DISP_ON][E_CANCEL_SHORT]      = transition_idle_disp_on;
    state_event_map[STATE_ALARM_TIMER_DISP_ON][E_ROOM_DARK]         = set_low_brightness;
    state_event_map[STATE_ALARM_TIMER_DISP_ON][E_ROOM_LIGHT]        = set_high_brightness;
    state_event_map[STATE_ALARM_TIMER_DISP_ON][E_VOLUP_SHORT]       = vol_up_short;
    state_event_map[STATE_ALARM_TIMER_DISP_ON][E_VOLUP_LONG]        = vol_up_long;
    state_event_map[STATE_ALARM_TIMER_DISP_ON][E_VOLDOWN_SHORT]     = vol_down_short;
    state_event_map[STATE_ALARM_TIMER_DISP_ON][E_VOLDOWN_LONG]      = vol_down_long;

    // ALARM TIMER TRIGGERED DISP OFF
    state_event_map[STATE_ALARM_TIMER_DISP_OFF][E_CANCEL_SHORT]     = transition_idle_disp_off;
    state_event_map[STATE_ALARM_TIMER_DISP_OFF][E_ROOM_DARK]        = set_low_brightness;
    state_event_map[STATE_ALARM_TIMER_DISP_OFF][E_ROOM_LIGHT]       = set_high_brightness;
    state_event_map[STATE_ALARM_TIMER_DISP_OFF][E_VOLUP_SHORT]      = vol_up_short;
    state_event_map[STATE_ALARM_TIMER_DISP_OFF][E_VOLUP_LONG]       = vol_up_long;
    state_event_map[STATE_ALARM_TIMER_DISP_OFF][E_VOLDOWN_SHORT]    = vol_down_short;
    state_event_map[STATE_ALARM_TIMER_DISP_OFF][E_VOLDOWN_LONG]     = vol_down_long;
}

static void set_state_right_short(void)
{
    if (curr_format == TRAD_24H)
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 1) % 6;
    else if (curr_format == TRAD_12H)
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 1) % 7;
    else if (curr_format == DOZ_DRN5 || curr_format == DOZ_SEMI)
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 1) % 5;
    else
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 1) % 4;
}
static void set_state_left_short(void)
{
    if (curr_format == TRAD_24H)
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 6 - 1) % 6;
    else if (curr_format == TRAD_12H)
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 7 - 1) % 7;
    else if (curr_format == DOZ_DRN5 || curr_format == DOZ_SEMI)
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 5 - 1) % 5;
    else
        g_clock_fsm.ctx->digit_sel = (g_clock_fsm.ctx->digit_sel + 4 - 1) % 4;
}
static void set_state_up_short(void)
{
    digits_changed = true;
    digit_value_increase(curr_format, &g_clock_fsm.ctx->digit_vals[g_clock_fsm.ctx->digit_sel], g_clock_fsm.ctx->digit_sel);
}
static void set_state_down_short(void)
{
    digits_changed = true;
    digit_value_decrease(curr_format, &g_clock_fsm.ctx->digit_vals[g_clock_fsm.ctx->digit_sel], g_clock_fsm.ctx->digit_sel);
}
static void set_low_brightness(void)
{
    Display_SetBrightness(LOW_BRIGHTNESS);
}
static void set_high_brightness(void)
{
    Display_SetBrightness(HIGH_BRIGHTNESS);
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
static void toggle_alarm_set(void)
{
    g_clock_fsm.ctx->alarm_set = !g_clock_fsm.ctx->alarm_set;
    Rtc_EnableAlarm(ALARM, g_clock_fsm.ctx->alarm_set);

    if (g_clock_fsm.ctx->alarm_set && g_clock_fsm.ctx->timer_alarm_displayed == DISPLAY_TIMER && !g_clock_fsm.ctx->timer_set)
        g_clock_fsm.ctx->timer_alarm_displayed = DISPLAY_ALARM;
    else if (!g_clock_fsm.ctx->alarm_set && g_clock_fsm.ctx->timer_alarm_displayed == DISPLAY_ALARM && g_clock_fsm.ctx->timer_set)
        g_clock_fsm.ctx->timer_alarm_displayed = DISPLAY_TIMER;
}
static void toggle_timer_set(void)
{
    g_clock_fsm.ctx->timer_set = !g_clock_fsm.ctx->timer_set;
    if (g_clock_fsm.ctx->timer_set) {
        RtcTime timerTime;
        timer_end_ms = (g_clock_fsm.ctx->user_timer_ms + g_clock_fsm.ctx->time_ms) % TIME_24H_MS;
        msToRtcTime(timer_end_ms, &timerTime);
        Rtc_SetAlarm(&timerTime, TIMER);

        if (g_clock_fsm.ctx->timer_alarm_displayed == DISPLAY_ALARM)
            g_clock_fsm.ctx->timer_alarm_displayed = DISPLAY_TIMER;
    } else if (g_clock_fsm.ctx->timer_alarm_displayed == DISPLAY_TIMER && g_clock_fsm.ctx->alarm_set) {
        g_clock_fsm.ctx->timer_alarm_displayed = DISPLAY_ALARM;
    }
    
    Rtc_EnableAlarm(TIMER, g_clock_fsm.ctx->timer_set);
}
static void toggle_trad_mode(void) 
{
    if (curr_format == trad_format_list[trad_index])
        trad_index = (trad_index + 1) % 2;
    curr_format = trad_format_list[trad_index];

    if (g_clock_fsm.curr_state->state_code == STATE_SET_TIME) {
        g_clock_fsm.ctx->digit_sel = 0;
        transition_digits(curr_format, g_clock_fsm.ctx->user_time_ms, g_clock_fsm.ctx->digit_vals);
    } else if (g_clock_fsm.curr_state->state_code == STATE_SET_ALARM) {
        g_clock_fsm.ctx->digit_sel = 0;
        transition_digits(curr_format, g_clock_fsm.ctx->user_alarm_ms, g_clock_fsm.ctx->digit_vals);
    }
    digits_changed = false;

    Display_SetFormat(curr_format);
}
static void toggle_doz_mode(void) 
{
    if (curr_format == doz_format_list[doz_index])
        doz_index = (doz_index + 1) % 3;
    curr_format = doz_format_list[doz_index];

    if (g_clock_fsm.curr_state->state_code == STATE_SET_TIME) {
        g_clock_fsm.ctx->digit_sel = 0;
        transition_digits(curr_format, g_clock_fsm.ctx->user_time_ms, g_clock_fsm.ctx->digit_vals);
    } else if (g_clock_fsm.curr_state->state_code == STATE_SET_ALARM) {
        g_clock_fsm.ctx->digit_sel = 0;
        transition_digits(curr_format, g_clock_fsm.ctx->user_alarm_ms, g_clock_fsm.ctx->digit_vals);
    }
    digits_changed = false;

    if (curr_format == DOZ_DRN4 && g_clock_fsm.ctx->diurn_radix_pos == RADIX_POS5)
        g_clock_fsm.ctx->diurn_radix_pos = RADIX_POS4;

    Display_SetFormat(curr_format);
}
static void toggle_doz_timer(void)
{
    if (curr_format == TRAD_24H) {
        curr_format = DOZ_DRN5;
        g_clock_fsm.ctx->digit_sel = 0;
        transition_digits(curr_format, g_clock_fsm.ctx->user_timer_ms, g_clock_fsm.ctx->digit_vals);
    }
    digits_changed = false;

    Display_SetFormat(curr_format);
}
static void toggle_trad_timer(void)
{
    if (curr_format == DOZ_DRN5) {
        curr_format = TRAD_24H;
        g_clock_fsm.ctx->digit_sel = 0;
        transition_digits(curr_format, g_clock_fsm.ctx->user_timer_ms, g_clock_fsm.ctx->digit_vals);
    }
    digits_changed = false;

    Display_SetFormat(curr_format);
}
static void toggle_mode(void)
{
    Display_ToggleMode();
}
static void toggle_timer_alarm_displayed(void)
{
    if (g_clock_fsm.ctx->timer_alarm_displayed == DISPLAY_ALARM && g_clock_fsm.ctx->timer_set)
        g_clock_fsm.ctx->timer_alarm_displayed = DISPLAY_TIMER;
    else if (g_clock_fsm.ctx->timer_alarm_displayed == DISPLAY_TIMER && g_clock_fsm.ctx->alarm_set)
        g_clock_fsm.ctx->timer_alarm_displayed = DISPLAY_ALARM;
}
static void rtc_demo_reset(void)
{
    Rtc_SetTime(&demo_reset);
}
static void cancel_set_state(void)
{
    cancel_pressed = 1;
    transition(&s_idle_disp_on);
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
static void radix_pos_left(void)
{
    if (curr_format == DOZ_DRN4)
        g_clock_fsm.ctx->diurn_radix_pos = (g_clock_fsm.ctx->diurn_radix_pos + 5 - 1) % 5;
    else if (curr_format == DOZ_DRN5)
        g_clock_fsm.ctx->diurn_radix_pos = (g_clock_fsm.ctx->diurn_radix_pos + 6 - 1) % 6;
    else if (curr_format == DOZ_SEMI)
        g_clock_fsm.ctx->semi_diurn_radix_pos = (g_clock_fsm.ctx->semi_diurn_radix_pos <= 1) ? 5 : (g_clock_fsm.ctx->semi_diurn_radix_pos - 1);
}
static void radix_pos_right(void)
{
    if (curr_format == DOZ_DRN4)
        g_clock_fsm.ctx->diurn_radix_pos = (g_clock_fsm.ctx->diurn_radix_pos + 1) % 5;
    else if (curr_format == DOZ_DRN5)
        g_clock_fsm.ctx->diurn_radix_pos = (g_clock_fsm.ctx->diurn_radix_pos + 1) % 6;
    else if (curr_format == DOZ_SEMI)
        g_clock_fsm.ctx->semi_diurn_radix_pos = (g_clock_fsm.ctx->semi_diurn_radix_pos >= 5) ? 1 : (g_clock_fsm.ctx->semi_diurn_radix_pos + 1);
}

static void digit_value_increase(TimeFormats timeFormat, uint8_t *val, uint8_t sel)
{
    if (timeFormat == TRAD_12H)
    {
        if (sel == 0)
            *val = (*val + 1) % 2;
        else if (sel == 1) 
        {
            if (g_clock_fsm.ctx->digit_vals[0] == 1)
                *val = (*val + 1) % 3;
            else
                *val = (*val + 1) % 10;
        }
        else if (sel == 2 || sel == 4)
            *val = (*val + 1) % 6;
        else if (sel == 3 || sel == 5)
            *val = (*val + 1) % 10;
        else if (sel == 6) 
            *val = (*val + 1) % 2;


        if (g_clock_fsm.ctx->digit_vals[0] == 1 && g_clock_fsm.ctx->digit_vals[1] > 2)
                g_clock_fsm.ctx->digit_vals[1] = 0;

        if (g_clock_fsm.ctx->digit_vals[0] == 1 && g_clock_fsm.ctx->digit_vals[1] == 2 && g_clock_fsm.ctx->digit_vals[6] == 0) {
            g_clock_fsm.ctx->digit_vals[6] = 1;
        } else if (g_clock_fsm.ctx->digit_vals[0] == 0 && g_clock_fsm.ctx->digit_vals[1] == 0 && g_clock_fsm.ctx->digit_vals[6] == 1) {
            g_clock_fsm.ctx->digit_vals[6] = 0;
        }
    }
    else if (timeFormat == TRAD_24H)
    {
        if (sel == 0) 
            *val = (*val + 1) % 3;
        else if (sel == 1) 
        {
            if (g_clock_fsm.ctx->digit_vals[0] == 2)
                *val = (*val + 1) % 4;
            else
                *val = (*val + 1) % 10;
        }
        else if (sel == 2 || sel == 4)
            *val = (*val + 1) % 6;
        else if (sel == 3 || sel == 5)
            *val = (*val + 1) % 10;


        if (g_clock_fsm.ctx->digit_vals[0] == 2 && g_clock_fsm.ctx->digit_vals[1] > 3)
                g_clock_fsm.ctx->digit_vals[1] = 0;
    }
    else if (timeFormat == DOZ_DRN4)
    {
        *val = (*val + 1) % 12;
    }
    else if (timeFormat == DOZ_DRN5)
    {
        *val = (*val + 1) % 12;
    }
    else if (timeFormat == DOZ_SEMI)
    {
        if (sel == 0)
            *val = (*val + 1) % 2;
        else
            *val = (*val + 1) % 12;
    }
}
static void digit_value_decrease(TimeFormats timeFormat, uint8_t *val, uint8_t sel)
{
    if (timeFormat == TRAD_12H)
    {
        if (sel == 0)
            *val = (*val + 2 - 1) % 2;
        else if (sel == 1) 
        {
            if (g_clock_fsm.ctx->digit_vals[0] == 1)
                *val = (*val + 3 - 1) % 3;
            else
                *val = (*val + 10 - 1) % 10;
        }
        else if (sel == 2 || sel == 4)
            *val = (*val + 6 - 1) % 6;
        else if (sel == 3 || sel == 5)
            *val = (*val + 10 - 1) % 10;
        else if (sel == 6) 
        {
            *val = (*val + 1) % 2;
        }


        if (g_clock_fsm.ctx->digit_vals[0] == 1 && g_clock_fsm.ctx->digit_vals[1] > 2)
                g_clock_fsm.ctx->digit_vals[1] = 0;

        if (g_clock_fsm.ctx->digit_vals[0] == 1 && g_clock_fsm.ctx->digit_vals[1] == 2 && g_clock_fsm.ctx->digit_vals[6] == 0) {
            g_clock_fsm.ctx->digit_vals[6] = 1;
        } else if (g_clock_fsm.ctx->digit_vals[0] == 0 && g_clock_fsm.ctx->digit_vals[1] == 0 && g_clock_fsm.ctx->digit_vals[6] == 1) {
            g_clock_fsm.ctx->digit_vals[6] = 0;
        }
    }
    else if (timeFormat == TRAD_24H)
    {
        if (sel == 0)
            *val = (*val + 3 - 1) % 3;
        else if (sel == 1) 
        {
            if (g_clock_fsm.ctx->digit_vals[0] == 2)
                *val = (*val + 4 - 1) % 4;
            else
                *val = (*val + 10 - 1) % 10;
        }
        else if (sel == 2 || sel == 4)
            *val = (*val + 6 - 1) % 6;
        else if (sel == 3 || sel == 5)
            *val = (*val + 10 - 1) % 10;


        if (g_clock_fsm.ctx->digit_vals[0] == 2 && g_clock_fsm.ctx->digit_vals[1] > 3)
                g_clock_fsm.ctx->digit_vals[1] = 0;
    }
    else if (timeFormat == DOZ_DRN4)
    {
        *val = (*val + 12 - 1) % 12;
    }
    else if (timeFormat == DOZ_DRN5)
    {
        *val = (*val + 12 - 1) % 12;
    }
    else if (timeFormat == DOZ_SEMI)
    {
        if (sel == 0)
            *val = (*val + 2 - 1) % 2;
        else
            *val = (*val + 12 - 1) % 12;
    }
}

#ifdef NO_PLATFORM
int main(void)
{
    return 0;
}
#endif

