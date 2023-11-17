#include "doz_clock.h"

#include <stdio.h>

static void Default_Entry(DozClock *ctx);
static void Default_Update(DozClock *ctx);
static void Default_Exit(DozClock *ctx);
static void Default_OnBtnPress(DozClock *ctx);
static void ClockStateInit_Entry(DozClock *ctx);
static void ClockStateInit_Update(DozClock *ctx);
static void ClockState24h_Entry(DozClock *ctx);
static void ClockState24h_OnBtnPress(DozClock *ctx);
static void ClockStateDiurnal_Entry(DozClock *ctx);
static void ClockStateDiurnal_OnBtnPress(DozClock *ctx);

static void transition(State *next);

static DozClockFSM g_clock_fsm;

// State definitions
State clock_init = {.state_code = STATE_INIT,
                    .entry = ClockStateInit_Entry,
                    .exit = Default_Exit,
                    .update = ClockStateInit_Update,
                    .onBtnPress = Default_OnBtnPress};

State clock_24h = {.state_code = STATE_24H,
                   .entry = ClockState24h_Entry,
                   .exit = Default_Exit,
                   .update = Default_Update,
                   .onBtnPress = ClockState24h_OnBtnPress};

State clock_diurnal = {.state_code = STATE_DIURNAL,
                       .entry = ClockStateDiurnal_Entry,
                       .exit = Default_Exit,
                       .update = Default_Update,
                       .onBtnPress = ClockStateDiurnal_OnBtnPress};

// FSM Event Functions
void DozClock_Init(DozClock *ctx) {
  g_clock_fsm.ctx = ctx;
  g_clock_fsm.curr_state = &clock_init;
  g_clock_fsm.curr_state->entry(g_clock_fsm.ctx);
}
void DozClock_Update() { g_clock_fsm.curr_state->update(g_clock_fsm.ctx); }
void DozClock_BtnPress() {
  g_clock_fsm.curr_state->onBtnPress(g_clock_fsm.ctx);
}

// Generic State Functions
static void Default_Entry(DozClock *ctx) { UNUSED(ctx); }
static void Default_Update(DozClock *ctx) { UNUSED(ctx); }
static void Default_Exit(DozClock *ctx) { UNUSED(ctx); }
static void Default_OnBtnPress(DozClock *ctx) { UNUSED(ctx); }

// Init State Functions
static void ClockStateInit_Entry(DozClock *ctx) { Display_Init(ctx->display); }
static void ClockStateInit_Update(DozClock *ctx) { transition(&clock_24h); }

// 24H State Functions
static void ClockState24h_Entry(DozClock *ctx) {
  Display_UpdateTime(ctx->display, 100000, DEC24H);
}
static void ClockState24h_OnBtnPress(DozClock *ctx) {
  UNUSED(ctx);
  transition(&clock_diurnal);
}

// Diurnal Functions
static void ClockStateDiurnal_Entry(DozClock *ctx) {
  Display_UpdateTime(ctx->display, 100000, DIURNAL);
}
static void ClockStateDiurnal_OnBtnPress(DozClock *ctx) {
  UNUSED(ctx);
  transition(&clock_24h);
}

// Helper functions
static void transition(State *next) {
  g_clock_fsm.curr_state->exit(g_clock_fsm.ctx);
  g_clock_fsm.curr_state = next;
  g_clock_fsm.curr_state->entry(g_clock_fsm.ctx);
}

#ifdef NO_PLATFORM
int main(void) { return 0; }
#endif
