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

#define DEFAULT_FORMAT      TRAD_24H
#define DEFAULT_BRIGHTNESS  HIGH_BRIGHTNESS

#define LARGE_BITMAP_SIZE 96
#define SMALL_BITMAP_SIZE 56

typedef struct bitmap{
    RowNumber   num;
    uint8_t     *p_bitmap;
    uint8_t     bitmap_size;
} Bitmap;

typedef enum state_code_t
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
// State machine functions
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

// Bitmap creation functions
static void displayChar(Bitmap *row_bitmap, uint8_t char_index,
        uint8_t digit[], uint8_t digitSize);
static void displayTime(Bitmap *row_bitmap, uint32_t time_ms);
static void blinkDigit(Bitmap *row_bitmap, uint8_t char_index);
static void updateBitmap(Bitmap *row_bitmap, uint8_t index, uint8_t digit[], uint8_t digitSize);
static uint8_t checkDeadZones(uint8_t digit[], uint8_t digitSize);
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
    Bitmap definitions
*/
static uint8_t row1_pixels[SMALL_BITMAP_SIZE];
Bitmap row1_bitmap = {
    .num = ROW_1,
    .p_bitmap = row1_pixels,
    .bitmap_size = SMALL_BITMAP_SIZE
};

static uint8_t row2_pixels[LARGE_BITMAP_SIZE];
Bitmap row2_bitmap = {
    .num = ROW_2,
    .p_bitmap = row2_pixels,
    .bitmap_size = LARGE_BITMAP_SIZE
};

static uint8_t row3_pixels[SMALL_BITMAP_SIZE];
Bitmap row3_bitmap = {
    .num = ROW_3,
    .p_bitmap = row3_pixels,
    .bitmap_size = SMALL_BITMAP_SIZE,
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
volatile uint8_t blink_state = 0;

/*
    Public functions
*/
ClockStatus Display_Init(Display *self, ExternVars *vars)
{
    // Check for NULL pointers
    if (self->displayOff == NULL ||
            self->displayOn == NULL ||
            self->setBrightness == NULL ||
            self->setBitmap == NULL ||
            self->setColour == NULL ||
            self->show == NULL ||
            self->hide == NULL)
    {
        return CLOCK_FAIL;
    }
    self->clock_vars = vars;
    self->time_format = DEFAULT_FORMAT;
    self->brightness = DEFAULT_BRIGHTNESS;
    g_fsm.ctx = self;
    g_fsm.curr_state = &s_off;
    show_time_index = 0;

    // Set brightness
    g_fsm.ctx->setBrightness(g_fsm.ctx->brightness);

    // Start FSM
    g_fsm.curr_state->entry(g_fsm.ctx);
    return CLOCK_OK;
}

void Display_Update(void)
{
    g_fsm.curr_state->update(g_fsm.ctx);
}

void Display_PeriodicCallback(void)
{
    // Update status of any blinking digits
    blink_state = !blink_state;
}

void Display_Off(void)
{
    // Check if currently in a ShowTime state
    if (g_fsm.curr_state->state_code ==
        show_time_states[show_time_index]->state_code)
    {
        transition(&s_off);
    }
}

void Display_On(void)
{
    // Check if currently in Off state
    if (g_fsm.curr_state->state_code == STATE_OFF)
    {
        transition(show_time_states[show_time_index]);
    }
}

void Display_ToggleMode(void)
{
    // Check if currently in a ShowTime state
    if (g_fsm.curr_state->state_code ==
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
    if (g_fsm.curr_state->state_code ==
        show_time_states[show_time_index]->state_code)
    {
        transition(&s_set_time);
    }
}

void Display_SetTimer(void)
{
    // Check if currently in a ShowTime state
    if (g_fsm.curr_state->state_code ==
        show_time_states[show_time_index]->state_code)
    {
        transition(&s_set_timer);
    }
}

void Display_SetAlarm(void)
{
    // Check if currently in a ShowTime state
    if (g_fsm.curr_state->state_code ==
        show_time_states[show_time_index]->state_code)
    {
        transition(&s_set_alarm);
    }
}

void Display_ShowTime(void)
{
    // Check if currently in a valid Set state
    if (g_fsm.curr_state->state_code == STATE_SETALARM
        || g_fsm.curr_state->state_code == STATE_SETTIMER
        || g_fsm.curr_state->state_code == STATE_SETTIME)
    {
        transition(show_time_states[show_time_index]);
    }
}

void Display_SetFormat(TimeFormats format)
{
    g_fsm.ctx->time_format = format;
}

void Display_SetBrightness(BrightnessLevels brightness)
{
    g_fsm.ctx->brightness = brightness;
    g_fsm.ctx->setBrightness(g_fsm.ctx->brightness);
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
    ctx->displayOff();
}
static void Off_Update(Display *ctx)
{
    UNUSED(ctx);
}
static void ShowTime123_Entry(Display *ctx)
{
    ctx->show(ROW_1);
    ctx->show(ROW_2);
    ctx->show(ROW_3);
}
static void ShowTime23_Entry(Display *ctx)
{
    ctx->hide(ROW_1);
    ctx->show(ROW_2);
    ctx->show(ROW_3);
}
static void ShowTime12_Entry(Display *ctx)
{
    ctx->show(ROW_1);
    ctx->show(ROW_2);
    ctx->hide(ROW_3);
}
static void ShowTime2_Entry(Display *ctx)
{
    ctx->hide(ROW_1);
    ctx->show(ROW_2);
    ctx->hide(ROW_3);
}
static void ShowTime_Update(Display *ctx)
{
    UNUSED(ctx);
}
static void SetTime_Entry(Display *ctx)
{
    ctx->show(ROW_1);
    ctx->show(ROW_2);
    ctx->show(ROW_3);
}
static void SetTime_Update(Display *ctx)
{
    UNUSED(ctx);
}
static void SetTimer_Entry(Display *ctx)
{
    ctx->show(ROW_1);
    ctx->show(ROW_2);
    ctx->show(ROW_3);
}
static void SetTimer_Update(Display *ctx)
{
    UNUSED(ctx);
}
static void SetAlarm_Entry(Display *ctx)
{
    ctx->show(ROW_1);
    ctx->show(ROW_2);
    ctx->show(ROW_3);
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

static void displayChar(Bitmap *row_bitmap, uint8_t char_index, uint8_t digit[], uint8_t digitSize)
{
    updateBitmap(row_bitmap, char_index, digit, digitSize);
}

uint8_t checkDeadZones(uint8_t digit[], uint8_t digitSize) {
    uint8_t numDeadZones = 0;
    for (unsigned offset = 0; offset < 8; ++ offset) {
        for (unsigned row = 0; row < digitSize; ++row) {
            if (((digit[row] >> offset) & 0x1)) {
                return numDeadZones;
            }
        }
        ++numDeadZones;
    }
    return numDeadZones;
}

// Writes digit to row_bitmap starting at display index 'index'
void updateBitmap(Bitmap *rowBitmap, uint8_t index, uint8_t digit[], uint8_t digitSize) {
    uint8_t deadZoneColumns = checkDeadZones(digit, digitSize);
    uint8_t column = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t lhsChanges = (deadZoneColumns > bitIndex) ? (8-deadZoneColumns) : (8-bitIndex);
    uint8_t rhsChanges = (deadZoneColumns >= bitIndex) ? 0 : (bitIndex - deadZoneColumns);

    uint8_t byte;
    uint8_t offset;
    bool set;
    for (uint8_t row = 0; row < digitSize; ++row) {
        byte = column + row*8;

        uint8_t lhs = rowBitmap->p_bitmap[byte];
        offset = 7 - bitIndex;
        for (uint8_t i = 0; i < lhsChanges; ++i) {
            set = (digit[row] >> (7-i)) & 0x1;
            if (set) {
                lhs = lhs | (0x1 << offset);
            } else {
                lhs = lhs & ~(0x1 << offset);
            }
            --offset;
        }
        rowBitmap->p_bitmap[byte] = lhs;

        if ((column % 8 < 7) && (rhsChanges > 0)) {
            uint8_t rhs = rowBitmap->p_bitmap[byte + 1];
            offset = 7;
            for (uint8_t i = 0; i < rhsChanges; ++i) {
                set = (digit[row] >> (7-lhsChanges-i)) & 0x1;
                if (set) {
                    rhs = rhs | (0x1 << offset);
                } else {
                    rhs = rhs & ~(0x1 << offset);
                }
                --offset;
            }
            rowBitmap->p_bitmap[byte + 1] = rhs;
        }
    }
}

// Debugging
void testDisplay() {

    unsigned testn = 2;

    // ROW 1 TESTS >>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // D
    if (testn == 0) {
        displayChar(&row1_bitmap, A_ROW1_DISPLAY_INDEX, small_symbols[A_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row1_bitmap, T_ROW1_DISPLAY_INDEX, small_symbols[T_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row1_bitmap, EXCLAMATION_ROW1_DISPLAY_INDEX, small_symbols[EXCLAMATION_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row1_bitmap, D_ROW1_DISPLAY_INDEX, small_symbols[D_INDEX], SMALL_DIGIT_ROWS);
    }

    // S
    if (testn == 1) {
        displayChar(&row1_bitmap, A_ROW1_DISPLAY_INDEX, small_symbols[A_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row1_bitmap, T_ROW1_DISPLAY_INDEX, small_symbols[T_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row1_bitmap, EXCLAMATION_ROW1_DISPLAY_INDEX, small_symbols[EXCLAMATION_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row1_bitmap, S_ROW1_DISPLAY_INDEX, small_symbols[S_INDEX], SMALL_DIGIT_ROWS);
    }

    // Moon
    if (testn == 2) {
        displayChar(&row1_bitmap, A_ROW1_DISPLAY_INDEX, small_symbols[A_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row1_bitmap, T_ROW1_DISPLAY_INDEX, small_symbols[T_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row1_bitmap, EXCLAMATION_ROW1_DISPLAY_INDEX, small_symbols[EXCLAMATION_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row1_bitmap, MOON_ROW1_DISPLAY_INDEX, small_symbols[MOON_INDEX], SMALL_DIGIT_ROWS);
    }

    // Sun
    if (testn == 3) {
        displayChar(&row1_bitmap, A_ROW1_DISPLAY_INDEX, small_symbols[A_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row1_bitmap, T_ROW1_DISPLAY_INDEX, small_symbols[T_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row1_bitmap, EXCLAMATION_ROW1_DISPLAY_INDEX, small_symbols[EXCLAMATION_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row1_bitmap, SUN_ROW1_DISPLAY_INDEX, small_symbols[SUN_INDEX], SMALL_DIGIT_ROWS);
    }

    // ROW 2 TESTS >>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // Trad
    if (testn == 0) {
        displayChar(&row2_bitmap, SEMICOLON2_ROW2_DISPLAY_INDEX, large_numbers[SEMICOLON_INDEX], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, TRAD_DIGIT_1_ROW2_DISPLAY_INDEX, large_numbers[0], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, TRAD_DIGIT_2_ROW2_DISPLAY_INDEX, large_numbers[8], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, SEMICOLON1_ROW2_DISPLAY_INDEX, large_numbers[SEMICOLON_INDEX], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, TRAD_DIGIT_3_ROW2_DISPLAY_INDEX, large_numbers[3], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, TRAD_DIGIT_4_ROW2_DISPLAY_INDEX, large_numbers[4], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, TRAD_DIGIT_5_ROW2_DISPLAY_INDEX, large_numbers[0], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, TRAD_DIGIT_6_ROW2_DISPLAY_INDEX, large_numbers[6], LARGE_DIGIT_ROWS);
    }

    // Diurn4
    if (testn == 1) {
        displayChar(&row2_bitmap, DRN4_DIGIT_1_ROW2_DISPLAY_INDEX, large_numbers[1], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, DRN4_DIGIT_2_ROW2_DISPLAY_INDEX, large_numbers[4], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, DRN4_DIGIT_3_ROW2_DISPLAY_INDEX, large_numbers[3], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, DRN4_DIGIT_4_ROW2_DISPLAY_INDEX, large_numbers[2], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, RADIX_DRN4_ROW2_DISPLAY_INDEX, large_numbers[RADIX_INDEX], LARGE_DIGIT_ROWS);
    }

    // Diurn5
    if (testn == 2) {
        displayChar(&row2_bitmap, RADIX_DRN5_ROW2_DISPLAY_INDEX, large_numbers[RADIX_INDEX], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, DRN5_DIGIT_1_ROW2_DISPLAY_INDEX, large_numbers[1], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, DRN5_DIGIT_2_ROW2_DISPLAY_INDEX, large_numbers[4], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, DRN5_DIGIT_3_ROW2_DISPLAY_INDEX, large_numbers[3], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, DRN5_DIGIT_4_ROW2_DISPLAY_INDEX, large_numbers[2], LARGE_DIGIT_ROWS);
        displayChar(&row2_bitmap, DRN5_DIGIT_5_ROW2_DISPLAY_INDEX, large_numbers[2], LARGE_DIGIT_ROWS);
    }

    // ROW 3 TESTS >>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // Trad PM
    if (testn == 0) {
        displayChar(&row3_bitmap, SEMICOLON1_ROW3_DISPLAY_INDEX, small_numbers[SEMICOLON_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, SEMICOLON2_ROW3_DISPLAY_INDEX, small_numbers[SEMICOLON_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, PM_ROW3_DISPLAY_INDEX, small_symbols[PM_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, TRAD_DIGIT_1_ROW3_DISPLAY_INDEX, small_numbers[0], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, TRAD_DIGIT_2_ROW3_DISPLAY_INDEX, small_numbers[9], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, TRAD_DIGIT_3_ROW3_DISPLAY_INDEX, small_numbers[5], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, TRAD_DIGIT_4_ROW3_DISPLAY_INDEX, small_numbers[5], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, TRAD_DIGIT_5_ROW3_DISPLAY_INDEX, small_numbers[0], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, TRAD_DIGIT_6_ROW3_DISPLAY_INDEX, small_numbers[0], SMALL_DIGIT_ROWS);
    }

    // Trad AM
    if (testn == 1) {
        displayChar(&row3_bitmap, SEMICOLON1_ROW3_DISPLAY_INDEX, small_numbers[SEMICOLON_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, SEMICOLON2_ROW3_DISPLAY_INDEX, small_numbers[SEMICOLON_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, PM_ROW3_DISPLAY_INDEX, small_symbols[AM_INDEX], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, TRAD_DIGIT_1_ROW3_DISPLAY_INDEX, small_numbers[0], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, TRAD_DIGIT_2_ROW3_DISPLAY_INDEX, small_numbers[9], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, TRAD_DIGIT_3_ROW3_DISPLAY_INDEX, small_numbers[5], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, TRAD_DIGIT_4_ROW3_DISPLAY_INDEX, small_numbers[5], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, TRAD_DIGIT_5_ROW3_DISPLAY_INDEX, small_numbers[0], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, TRAD_DIGIT_6_ROW3_DISPLAY_INDEX, small_numbers[0], SMALL_DIGIT_ROWS);
    }

    // Diurn4
    if (testn == 2) {
        displayChar(&row3_bitmap, DRN4_DIGIT_1_ROW3_DISPLAY_INDEX, small_numbers[0], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, DRN4_DIGIT_2_ROW3_DISPLAY_INDEX, small_numbers[10], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, DRN4_DIGIT_3_ROW3_DISPLAY_INDEX, small_numbers[6], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, DRN4_DIGIT_4_ROW3_DISPLAY_INDEX, small_numbers[0], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, RADIX_DRN4_ROW3_DISPLAY_INDEX, small_numbers[RADIX_INDEX], SMALL_DIGIT_ROWS);
    }

    // Diurn5
    if (testn == 3) {
        displayChar(&row3_bitmap, DRN5_DIGIT_1_ROW3_DISPLAY_INDEX, small_numbers[0], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, DRN5_DIGIT_2_ROW3_DISPLAY_INDEX, small_numbers[10], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, DRN5_DIGIT_3_ROW3_DISPLAY_INDEX, small_numbers[6], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, DRN5_DIGIT_4_ROW3_DISPLAY_INDEX, small_numbers[0], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, DRN5_DIGIT_5_ROW3_DISPLAY_INDEX, small_numbers[0], SMALL_DIGIT_ROWS);
        displayChar(&row3_bitmap, RADIX_DRN5_ROW3_DISPLAY_INDEX, small_numbers[RADIX_INDEX], SMALL_DIGIT_ROWS);
    }

    printDisplay();
}

void printDisplay() {
    printf("Printing Rows >>>>>>>>>>>>>>>>>>>\n\n");

    uint8_t columnCounter = 0;
    printf("Row 1\n\n");
    for (uint8_t i = 0; i < SMALL_BITMAP_SIZE; ++i) {
        for (int8_t b = 7; b >= 0; --b) {
            if ((row1_bitmap.p_bitmap[i] >> b) & 0x1) {
                printf("o");
            } else {
                printf(".");
            }
        }
        ++columnCounter;
        if (columnCounter >= 8) {
            columnCounter = 0;
            printf("\n");
        }
    }
    printf("\n");

    columnCounter = 0;
    printf("Row 2\n\n");
    for (uint8_t i = 0; i < LARGE_BITMAP_SIZE; ++i) {
        for (int8_t b = 7; b >= 0; --b) {
            if ((row2_bitmap.p_bitmap[i] >> b) & 0x1) {
                printf("o");
            } else {
                printf(".");
            }
        }
        ++columnCounter;
        if (columnCounter >= 8) {
            columnCounter = 0;
            printf("\n");
        }
    }
    printf("\n");

    columnCounter = 0;
    printf("Row 3\n\n");
    for (uint8_t i = 0; i < SMALL_BITMAP_SIZE; ++i) {
        for (int8_t b = 7; b >= 0; --b) {
            if ((row3_bitmap.p_bitmap[i] >> b) & 0x1) {
                printf("o");
            } else {
                printf(".");
            }
        }
        ++columnCounter;
        if (columnCounter >= 8) {
            columnCounter = 0;
            printf("\n");
        }
    }
    printf("\n");
}
