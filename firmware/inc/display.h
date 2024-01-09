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
} Display;

void Display_Init(Display *self, ExternVars *vars);
void Display_Update(void);
void Display_PeriodicCallback(void);
void Display_Off(void);
void Display_On(void);
void Display_ToggleMode(void);
void Display_SetTime(void);
void Display_SetTimer(void);
void Display_SetAlarm(void);
void Display_ShowTime(void);
#endif  // FIRMWARE_INC_DISPLAY_H_
