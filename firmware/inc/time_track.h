/*
 * time.h
 *
 *  Created on: Nov. 18, 2023
 *      Author: lemck
 */

#ifndef FIRMWARE_INC_TIME_TRACK_H_
#define FIRMWARE_INC_TIME_TRACK_H_

#include "clock_types.h"
#include "gps.h"
#include "rtc_module.h"

#define TIME_24H_MS      86400000

typedef enum time_shift_t
{
    SHIFT_0,
    SHIFT_6,
    SHIFT_12,
    SHIFT_18,
    NUM_SHIFTS
} TimeShift;

ClockStatus TimeTrack_Init();
ClockStatus TimeTrack_SyncToRtc();
ClockStatus TimeTrack_Update();
ClockStatus TimeTrack_PeriodicCallback(uint32_t period_ms);
ClockStatus TimeTrack_GetTimeMs(uint32_t *output_ms);
ClockStatus TimeTrack_UseTimeShift(TimeShift shift);
uint32_t TimeTrack_ConvertToShiftedTime(uint32_t reg_time_ms);
uint32_t TimeTrack_ConvertToNonShiftedTime(uint32_t shift_time_ms);
bool TimeTrack_ShiftToDifferentDay();

void msToRtcTime(uint32_t time_ms, RtcTime *time);

#endif /* FIRMWARE_INC_TIME_TRACK_H_ */
