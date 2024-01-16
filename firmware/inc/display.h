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

typedef struct extern_vars_t
{
    uint32_t    *time_ms;
    uint32_t    *user_time_ms;
    uint32_t    *user_alarm_ms;
    uint32_t    *user_timer_ms;
    uint8_t     *digit_sel;
    uint8_t     *digit_val;
    uint8_t     *error_code;
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
    void (*setColour)(uint8_t region_id, uint8_t colour_id);
    void (*show)(uint8_t region_id);
    void (*hide)(uint8_t region_id);
} Display;

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

// Testing
void printDisplay();
#endif  // FIRMWARE_INC_DISPLAY_H_
