/*
 * display.h
 * Software to operate the clock display
 *
 *  Created on: Jan 08, 2024
 *      Author: Matt L.
 */

#ifndef FIRMWARE_INC_DISPLAY_H_
#define FIRMWARE_INC_DISPLAY_H_

#include "clock_types.h"

// Row 1 Display Indeces
#define A_ROW1_DISPLAY_INDEX            1
#define T_ROW1_DISPLAY_INDEX            6
#define EXCLAMATION_ROW1_DISPLAY_INDEX  11
#define D_ROW1_DISPLAY_INDEX            60
#define S_ROW1_DISPLAY_INDEX            60
#define AM_PM_ROW1_DISPLAY_INDEX        55
#define FORMAT_ROW1_DISPLAY_INDEX       57

// Row 2 Display Indeces
#define SEMICOLON1_ROW2_DISPLAY_INDEX    20
#define SEMICOLON2_ROW2_DISPLAY_INDEX    42
#define RADIX_DRN4_ROW2_DISPLAY_INDEX    41
#define RADIX_DRN5_ROW2_DISPLAY_INDEX    36
#define RADIX_SEMI_ROW2_DISPLAY_INDEX    26

#define TRAD_DIGIT_1_ROW2_DISPLAY_INDEX     1
#define TRAD_DIGIT_2_ROW2_DISPLAY_INDEX     11
#define TRAD_DIGIT_3_ROW2_DISPLAY_INDEX     23
#define TRAD_DIGIT_4_ROW2_DISPLAY_INDEX     33
#define TRAD_DIGIT_5_ROW2_DISPLAY_INDEX     45
#define TRAD_DIGIT_6_ROW2_DISPLAY_INDEX     55

#define DRN4_DIGIT_1_ROW2_DISPLAY_INDEX     11
#define DRN4_DIGIT_2_ROW2_DISPLAY_INDEX     21
#define DRN4_DIGIT_3_ROW2_DISPLAY_INDEX     31
#define DRN4_DIGIT_4_ROW2_DISPLAY_INDEX     45

#define DRN5_DIGIT_1_ROW2_DISPLAY_INDEX     6
#define DRN5_DIGIT_2_ROW2_DISPLAY_INDEX     16
#define DRN5_DIGIT_3_ROW2_DISPLAY_INDEX     26
#define DRN5_DIGIT_4_ROW2_DISPLAY_INDEX     40
#define DRN5_DIGIT_5_ROW2_DISPLAY_INDEX     50

#define SEMI_DIGIT_1_ROW2_DISPLAY_INDEX     6
#define SEMI_DIGIT_2_ROW2_DISPLAY_INDEX     16
#define SEMI_DIGIT_3_ROW2_DISPLAY_INDEX     30
#define SEMI_DIGIT_4_ROW2_DISPLAY_INDEX     40
#define SEMI_DIGIT_5_ROW2_DISPLAY_INDEX     50

// Row 3 Display Indeces
#define SEMICOLON1_ROW3_DISPLAY_INDEX   21
#define SEMICOLON2_ROW3_DISPLAY_INDEX   35
#define RADIX_DRN4_ROW3_DISPLAY_INDEX   37
#define RADIX_DRN5_ROW3_DISPLAY_INDEX   34
#define RADIX_SEMI_ROW3_DISPLAY_INDEX   28
#define AM_PM_ROW3_DISPLAY_INDEX        49

#define TRAD_DIGIT_1_ROW3_DISPLAY_INDEX     9
#define TRAD_DIGIT_2_ROW3_DISPLAY_INDEX     15
#define TRAD_DIGIT_3_ROW3_DISPLAY_INDEX     23
#define TRAD_DIGIT_4_ROW3_DISPLAY_INDEX     29
#define TRAD_DIGIT_5_ROW3_DISPLAY_INDEX     37
#define TRAD_DIGIT_6_ROW3_DISPLAY_INDEX     43

#define DRN4_DIGIT_1_ROW3_DISPLAY_INDEX     19
#define DRN4_DIGIT_2_ROW3_DISPLAY_INDEX     25
#define DRN4_DIGIT_3_ROW3_DISPLAY_INDEX     31
#define DRN4_DIGIT_4_ROW3_DISPLAY_INDEX     39

#define DRN5_DIGIT_1_ROW3_DISPLAY_INDEX     16
#define DRN5_DIGIT_2_ROW3_DISPLAY_INDEX     22
#define DRN5_DIGIT_3_ROW3_DISPLAY_INDEX     28
#define DRN5_DIGIT_4_ROW3_DISPLAY_INDEX     36
#define DRN5_DIGIT_5_ROW3_DISPLAY_INDEX     42

#define SEMI_DIGIT_1_ROW3_DISPLAY_INDEX     16
#define SEMI_DIGIT_2_ROW3_DISPLAY_INDEX     22
#define SEMI_DIGIT_3_ROW3_DISPLAY_INDEX     30
#define SEMI_DIGIT_4_ROW3_DISPLAY_INDEX     36
#define SEMI_DIGIT_5_ROW3_DISPLAY_INDEX     42

#define NUM_SHOWTIME_STATES 4

#define LARGE_BITMAP_SIZE 96
#define SMALL_BITMAP_SIZE 56

typedef enum brightness_levels_t
{
    LOW_BRIGHTNESS = 30,
    MED_BRIGHTNESS = 130,
    HIGH_BRIGHTNESS = 255,
} BrightnessLevels;

typedef enum time_formats_t
{
    TRAD_24H,
    TRAD_12H,
    DOZ_DRN4,
    DOZ_DRN5,
    DOZ_SEMI,
} TimeFormats;

typedef enum row_number_t
{
    ROW_1 = 0,
    ROW_2 = 1,
    ROW_3 = 2
} RowNumber;

typedef struct extern_vars_t
{
    uint32_t    *time_ms;
    uint32_t    *user_time_ms;
    uint32_t    *user_alarm_ms;
    uint32_t    *user_timer_ms;
    uint8_t     *digit_sel;
    uint8_t     *digit_vals;
    ClockStatus *error_code;
    bool        *alarm_set;
    bool        *timer_set;
    bool        *alarm_triggered;
    bool        *timer_triggered;
    bool        *show_error;
} ExternVars;

typedef struct display_t
{
    ExternVars  *clock_vars;
    TimeFormats time_format;
    uint8_t     brightness;

    void (*displayOff)(void);
    void (*displayOn)(void);
    void (*setBrightness)(uint8_t brightness);
    void (*setBitmap)(uint8_t region_id, uint8_t *bitmap);
    void (*setColour)(uint8_t region_id, Colour colour_id);
    void (*show)(uint8_t region_id);
    void (*hide)(uint8_t region_id);
} Display;

typedef struct bitmap{
    RowNumber   num;
    uint8_t     *p_bitmap;
    uint8_t     bitmap_size;
} Bitmap;
typedef enum display_state_code_t
{
    STATE_OFF,
    STATE_SHOWTIME123,
    STATE_SHOWTIME23,
    STATE_SHOWTIME12,
    STATE_SHOWTIME2,
    STATE_SETTIME,
    STATE_SETTIMER,
    STATE_SETALARM
} DisplayStateCode;

typedef struct display_state_t
{
    DisplayStateCode state_code;

    void (*entry)(Display *ctx);
    void (*update)(Display *ctx);
    void (*exit)(Display *ctx);
} DisplayState;

typedef struct display_state_machine_t {
    DisplayState *curr_state;
    Display *ctx;
} DisplayFSM;

ClockStatus Display_Init(Display *self, ExternVars *vars);
void Display_Update(void);
void Display_PeriodicCallback(void);
void Display_Off(void);
void Display_On(void);
void Display_ToggleMode(void);
void Display_SetTime(void);
void Display_SetTimer(void);
void Display_SetAlarm(void);
void Display_ShowTime(void);
void Display_SetFormat(TimeFormats format);
void Display_SetBrightness(BrightnessLevels brightness);
#endif  // FIRMWARE_INC_DISPLAY_H_
