/*
 * display.c
 * Software to operate the clock display
 *
 *  Created on: Jan 08, 2024
 *      Author: Matt L.
 */

#include "display.h"
#include "bitmaps.h"

#define NUM_SHOWTIME_STATES 4

typedef enum 
{
    STATE_OFF, 
    STATE_SHOWTIME123, 
    STATE_SHOWTIME23, 
    STATE_SHOWTIME12, 
    STATE_SHOWTIME2,
    STATE_SETTIME,
    STATE_SETTIMER,
    STATE_SETALARM
} StateCode;

typedef struct state_t
{
    StateCode state_code;

    void (*entry)(Display *ctx);
    void (*update)(Display *ctx);
    void (*exit)(Display *ctx);
} State;

typedef struct state_machine_t {
    State *curr_state;
    Display *ctx;
} DisplayFSM;

/*
    Private function definitions
*/
static void Default_Entry(Display *ctx);
static void Default_Update(Display *ctx);
static void Default_Exit(Display *ctx);
static void Off_Entry(Display *ctx);
static void Off_Update(Display *ctx);
static void ShowTime123_Entry(Display *ctx);
static void ShowTime23_Entry(Display *ctx);
static void ShowTime12_Entry(Display *ctx);
static void ShowTime2_Entry(Display *ctx);
static void ShowTime_Update(Display *ctx);
static void SetTime_Entry(Display *ctx);
static void SetTime_Update(Display *ctx);
static void SetTimer_Entry(Display *ctx);
static void SetTimer_Update(Display *ctx);
static void SetAlarm_Entry(Display *ctx);
static void SetAlarm_Update(Display *ctx);

static void transition(State *next);

/*
    State definitions
*/
State s_off = 
{
    .state_code = STATE_OFF,
    .entry = Default_Entry,
    .update = Default_Update,
    .exit = Default_Exit,
};

State s_show_time123 =
{
    .state_code = STATE_SHOWTIME123,
    .entry = ShowTime123_Entry,
    .update = ShowTime_Update,
    .exit = Default_Exit,
};

State s_show_time23 =
{
    .state_code = STATE_SHOWTIME23,
    .entry = ShowTime23_Entry,
    .update = ShowTime_Update,
    .exit = Default_Exit,
};

State s_show_time12 =
{
    .state_code = STATE_SHOWTIME12,
    .entry = ShowTime12_Entry,
    .update = ShowTime_Update,
    .exit = Default_Exit,
};

State s_show_time2 =
{
    .state_code = STATE_SHOWTIME2,
    .entry = ShowTime2_Entry,
    .update = ShowTime_Update,
    .exit = Default_Exit,
};

State s_set_time =
{
    .state_code = STATE_SETTIME,
    .entry = SetTime_Entry,
    .update = SetTime_Update,
    .exit = Default_Exit,
};

State s_set_timer =
{
    .state_code = STATE_SETTIMER,
    .entry = SetTimer_Entry,
    .update = SetTimer_Update,
    .exit = Default_Exit,
};

State s_set_alarm =
{
    .state_code = STATE_SETALARM,
    .entry = SetAlarm_Entry,
    .update = SetAlarm_Update,
    .exit = Default_Exit,
};

/*
    Private variables
*/
static DisplayFSM g_fsm = {0};
static State *show_time_states[NUM_SHOWTIME_STATES] = 
{
    &s_show_time123, 
    &s_show_time23,
    &s_show_time12,
    &s_show_time2
};
static uint8_t show_time_index = 0;

/*
    Public functions
*/
void Display_Init(Display *self, ExternVars *vars)
{
    self->clock_vars = vars;
    g_fsm.ctx = self;
    g_fsm.curr_state = &s_off;
    show_time_index = 0;
    g_fsm.curr_state->entry(g_fsm.ctx);
}

void Display_Update(void)
{
    g_fsm.curr_state->update(g_fsm.ctx);
}

void Display_PeriodicCallback(void)
{

}

void Display_Off(void)
{
    // Check if currently in a ShowTime state
    if(g_fsm.curr_state->state_code == 
        show_time_states[show_time_index]->state_code)
    {
        transition(&s_off);
    }
}

void Display_On(void)
{
    // Check if currently in Off state
    if(g_fsm.curr_state->state_code == STATE_OFF)
    {
        transition(show_time_states[show_time_index]);
    }
}

void Display_ToggleMode(void)
{
    // Check if currently in a ShowTime state
    if(g_fsm.curr_state->state_code == 
        show_time_states[show_time_index]->state_code)
    {
        // Transition to the next ShowTime state
        show_time_index = (show_time_index + 1) % NUM_SHOWTIME_STATES;
        transition(show_time_states[show_time_index]);
    }
}

void Display_SetTime(void)
{
    // Check if currently in a ShowTime state
    if(g_fsm.curr_state->state_code == 
        show_time_states[show_time_index]->state_code)
    {
        transition(&s_set_time);
    }
}

void Display_SetTimer(void)
{
    // Check if currently in a ShowTime state
    if(g_fsm.curr_state->state_code == 
        show_time_states[show_time_index]->state_code)
    {
        transition(&s_set_timer);
    }
}

void Display_SetAlarm(void)
{
    // Check if currently in a ShowTime state
    if(g_fsm.curr_state->state_code == 
        show_time_states[show_time_index]->state_code)
    {
        transition(&s_set_alarm);
    }
}

void Display_ShowTime(void)
{
    // Check if currently in a valid Set state
    if(g_fsm.curr_state->state_code == STATE_SETALARM
        || g_fsm.curr_state->state_code == STATE_SETTIMER
        || g_fsm.curr_state->state_code == STATE_SETTIME)
    {
        transition(show_time_states[show_time_index]);
    }
}

/*
    Private functions
*/
void Default_Entry(Display *ctx)
{
    UNUSED(ctx);
}
void Default_Update(Display *ctx)
{
    UNUSED(ctx);
}
void Default_Exit(Display *ctx)
{
    UNUSED(ctx);
}
static void Off_Entry(Display *ctx)
{
    UNUSED(ctx);
}
static void Off_Update(Display *ctx)
{
    UNUSED(ctx);
}
static void ShowTime123_Entry(Display *ctx)
{
    UNUSED(ctx);
}
static void ShowTime23_Entry(Display *ctx)
{
    UNUSED(ctx);
}
static void ShowTime12_Entry(Display *ctx)
{
    UNUSED(ctx);
}
static void ShowTime2_Entry(Display *ctx)
{
    UNUSED(ctx);
}
static void ShowTime_Update(Display *ctx)
{
    UNUSED(ctx);
}
static void SetTime_Entry(Display *ctx)
{
    UNUSED(ctx);
}
static void SetTime_Update(Display *ctx)
{
    UNUSED(ctx);
}
static void SetTimer_Entry(Display *ctx)
{
    UNUSED(ctx);
}
static void SetTimer_Update(Display *ctx)
{
    UNUSED(ctx);
}
static void SetAlarm_Entry(Display *ctx)
{
    UNUSED(ctx);
}
static void SetAlarm_Update(Display *ctx)
{
    UNUSED(ctx);
}

void transition(State *next)
{
    g_fsm.curr_state->exit(g_fsm.ctx);
    g_fsm.curr_state = next;
    g_fsm.curr_state->entry(g_fsm.ctx);
}

#ifdef NO_PLATFORM
int main(void)
{
    return 0;
}
#endif