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

#define RADIX_POS0_ROW2_5DIGIT_DISPLAY_INDEX    6
#define RADIX_POS1_ROW2_5DIGIT_DISPLAY_INDEX    16
#define RADIX_POS2_ROW2_5DIGIT_DISPLAY_INDEX    26
#define RADIX_POS3_ROW2_5DIGIT_DISPLAY_INDEX    36
#define RADIX_POS4_ROW2_5DIGIT_DISPLAY_INDEX    46
#define RADIX_POS5_ROW2_5DIGIT_DISPLAY_INDEX    56

#define RADIX_POS0_ROW2_4DIGIT_DISPLAY_INDEX    11
#define RADIX_POS1_ROW2_4DIGIT_DISPLAY_INDEX    21
#define RADIX_POS2_ROW2_4DIGIT_DISPLAY_INDEX    31
#define RADIX_POS3_ROW2_4DIGIT_DISPLAY_INDEX    41
#define RADIX_POS4_ROW2_4DIGIT_DISPLAY_INDEX    51

#define TRAD_DIGIT_1_ROW2_DISPLAY_INDEX     1
#define TRAD_DIGIT_2_ROW2_DISPLAY_INDEX     11
#define TRAD_DIGIT_3_ROW2_DISPLAY_INDEX     23
#define TRAD_DIGIT_4_ROW2_DISPLAY_INDEX     33
#define TRAD_DIGIT_5_ROW2_DISPLAY_INDEX     45
#define TRAD_DIGIT_6_ROW2_DISPLAY_INDEX     55

#define TRAD_HIDE_SECONDS_INDEX_SHIFT       11

#define DRN4_DIGIT_1_ROW2_DISPLAY_INDEX     11
#define DRN4_DIGIT_2_ROW2_DISPLAY_INDEX     21
#define DRN4_DIGIT_3_ROW2_DISPLAY_INDEX     31
#define DRN4_DIGIT_4_ROW2_DISPLAY_INDEX     41

#define DRN5_DIGIT_1_ROW2_DISPLAY_INDEX     6
#define DRN5_DIGIT_2_ROW2_DISPLAY_INDEX     16
#define DRN5_DIGIT_3_ROW2_DISPLAY_INDEX     26
#define DRN5_DIGIT_4_ROW2_DISPLAY_INDEX     36
#define DRN5_DIGIT_5_ROW2_DISPLAY_INDEX     46

#define SEMI_DIGIT_1_ROW2_DISPLAY_INDEX     6
#define SEMI_DIGIT_2_ROW2_DISPLAY_INDEX     16
#define SEMI_DIGIT_3_ROW2_DISPLAY_INDEX     26
#define SEMI_DIGIT_4_ROW2_DISPLAY_INDEX     36
#define SEMI_DIGIT_5_ROW2_DISPLAY_INDEX     46

// Row 3 Display Indeces
#define SEMICOLON1_ROW3_DISPLAY_INDEX   24
#define SEMICOLON2_ROW3_DISPLAY_INDEX   38
#define AM_PM_ROW3_DISPLAY_INDEX        52

#define RADIX_POS0_ROW3_5DIGIT_DISPLAY_INDEX    16
#define RADIX_POS1_ROW3_5DIGIT_DISPLAY_INDEX    22
#define RADIX_POS2_ROW3_5DIGIT_DISPLAY_INDEX    28
#define RADIX_POS3_ROW3_5DIGIT_DISPLAY_INDEX    34
#define RADIX_POS4_ROW3_5DIGIT_DISPLAY_INDEX    40
#define RADIX_POS5_ROW3_5DIGIT_DISPLAY_INDEX    46

#define RADIX_POS0_ROW3_4DIGIT_DISPLAY_INDEX    19
#define RADIX_POS1_ROW3_4DIGIT_DISPLAY_INDEX    25
#define RADIX_POS2_ROW3_4DIGIT_DISPLAY_INDEX    31
#define RADIX_POS3_ROW3_4DIGIT_DISPLAY_INDEX    37
#define RADIX_POS4_ROW3_4DIGIT_DISPLAY_INDEX    43

#define TRAD_DIGIT_1_ROW3_DISPLAY_INDEX     12
#define TRAD_DIGIT_2_ROW3_DISPLAY_INDEX     18
#define TRAD_DIGIT_3_ROW3_DISPLAY_INDEX     26
#define TRAD_DIGIT_4_ROW3_DISPLAY_INDEX     32
#define TRAD_DIGIT_5_ROW3_DISPLAY_INDEX     40
#define TRAD_DIGIT_6_ROW3_DISPLAY_INDEX     46

#define DRN4_DIGIT_1_ROW3_DISPLAY_INDEX     19
#define DRN4_DIGIT_2_ROW3_DISPLAY_INDEX     25
#define DRN4_DIGIT_3_ROW3_DISPLAY_INDEX     31
#define DRN4_DIGIT_4_ROW3_DISPLAY_INDEX     37

#define DRN5_DIGIT_1_ROW3_DISPLAY_INDEX     16
#define DRN5_DIGIT_2_ROW3_DISPLAY_INDEX     22
#define DRN5_DIGIT_3_ROW3_DISPLAY_INDEX     28
#define DRN5_DIGIT_4_ROW3_DISPLAY_INDEX     34
#define DRN5_DIGIT_5_ROW3_DISPLAY_INDEX     40

#define SEMI_DIGIT_1_ROW3_DISPLAY_INDEX     16
#define SEMI_DIGIT_2_ROW3_DISPLAY_INDEX     22
#define SEMI_DIGIT_3_ROW3_DISPLAY_INDEX     28
#define SEMI_DIGIT_4_ROW3_DISPLAY_INDEX     34
#define SEMI_DIGIT_5_ROW3_DISPLAY_INDEX     40

#define NUM_SHOWTIME_STATES 4

#define LARGE_BITMAP_SIZE 96
#define SMALL_BITMAP_SIZE 56

#define ROW2_RADIX_OFFSET   4
#define ROW3_RADIX_OFFSET   2

typedef enum brightness_levels_t
{
    LOW_BRIGHTNESS = 0,
    MED_BRIGHTNESS = 30,
    HIGH_BRIGHTNESS = 150,
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

typedef enum radix_pos_t
{
    RADIX_POS0,
    RADIX_POS1,
    RADIX_POS2,
    RADIX_POS3,
    RADIX_POS4,
    RADIX_POS5,

    NUM_RADIX_POS
} RadixPos;

typedef enum timer_alarm_displayed_t
{
    DISPLAY_TIMER,
    DISPLAY_ALARM
} TimerAlarmDisplayed;

typedef struct extern_vars_t
{
    uint32_t    *time_ms;
    uint32_t    *user_time_ms;
    uint32_t    *user_alarm_ms;
    uint32_t    *user_timer_ms;
    int32_t     *rtc_calib;
    uint8_t     *digit_sel;
    uint8_t     *digit_vals;
    uint8_t     *diurn_radix_pos;
    uint8_t     *semi_diurn_radix_pos;
    ClockStatus *error_code;
    bool        *alarm_set;
    bool        *timer_set;
    bool        *alarm_triggered;
    bool        *timer_triggered;
    bool        *show_error;
    bool        *timer_alarm_displayed;
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
    STATE_SETALARM,
    STATE_SETCALIB,
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
void Display_SetCalib(void);
#endif  // FIRMWARE_INC_DISPLAY_H_
