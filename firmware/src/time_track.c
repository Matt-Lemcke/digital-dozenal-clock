/*
 * time.c
 *
 *  Created on: Nov. 18, 2023
 *      Author: lemck
 */


#include <rtc_module.h>
#include "time_track.h"

#include "gps.h"
#include "rtc_module.h"

uint8_t check_rtc = 0;
uint8_t gps_lost = 0;
uint16_t n = 0;

uint32_t time_ms = 0;
static RtcTime rtc_time = { 0 }, prev_rtc_time = { 0 };
static GpsTime gps_time = { 0 };
static TimeShift current_shift = SHIFT_0;

static const uint32_t timeshift_lookup_ms[NUM_SHIFTS] = {
        [SHIFT_0] = 0,
        [SHIFT_6] = 6 * 3600 * 1000,
        [SHIFT_12] = 12 * 3600 * 1000,
        [SHIFT_18] = 18 * 3600 * 1000
};

static uint8_t rtcTimesEqual(RtcTime *time_a, RtcTime *time_b);
static uint32_t rtcTimeToMs(RtcTime *time);
static uint32_t gpsTimeToMs(GpsTime *time);

ClockStatus TimeTrack_Init()
{
    n = 0;
    if (Rtc_GetTime(&rtc_time) != CLOCK_OK)
    {
        return CLOCK_FAIL;
    }
    time_ms = rtcTimeToMs(&rtc_time);
    prev_rtc_time = rtc_time;
    return CLOCK_OK;
}

ClockStatus TimeTrack_SyncToRtc()
{
    if (Rtc_GetTime(&rtc_time) != CLOCK_OK)
    {
        return CLOCK_FAIL;
    }
    prev_rtc_time = rtc_time;
    time_ms = rtcTimeToMs(&rtc_time);
    return CLOCK_OK;
}

ClockStatus TimeTrack_Update()
{
    if (check_rtc)
    {
        check_rtc = 0;
        if (Rtc_GetTime(&rtc_time) != CLOCK_OK)
        {
            return CLOCK_FAIL;
        }
        if (!rtcTimesEqual(&rtc_time, &prev_rtc_time))
        {
            // Re-sync internal time to RTC when it updates
            time_ms = rtcTimeToMs(&rtc_time);
            prev_rtc_time = rtc_time;
            n++;
        }
    }
    if (n >= 3600)
    {
        // Attempt to re-sync internal time and RTC to GPS every hour
        if (Gps_Connected() == CLOCK_OK)
        {
            // GPS connected
            gps_lost = 0;
            gps_time = Gps_GetTime();

            time_ms = gpsTimeToMs(&gps_time);   // Sync internal time to GPS

            msToRtcTime(time_ms, &rtc_time);

            // Sync RTC to GPS
            if (Rtc_SetTime(&rtc_time) != CLOCK_OK)
            {
                return CLOCK_FAIL;
            }
            prev_rtc_time = rtc_time;
            n = 0;
        }
        else
        {
            // GPS disconnected
            gps_lost = 1;
        }
    }
    return CLOCK_OK;
}

ClockStatus TimeTrack_PeriodicCallback(uint32_t period_ms)
{
    time_ms = (time_ms + period_ms) % TIME_24H_MS;
    check_rtc = 1;
    return CLOCK_OK;
}

ClockStatus TimeTrack_GetTimeMs(uint32_t *output_ms)
{
    *output_ms = (time_ms + timeshift_lookup_ms[current_shift]) % TIME_24H_MS;
    return CLOCK_OK;
}

ClockStatus TimeTrack_UseTimeShift(TimeShift shift)
{
    current_shift = shift;
    return CLOCK_OK;
}

uint32_t TimeTrack_ConvertToShiftedTime(uint32_t reg_time_ms)
{
    return (reg_time_ms + timeshift_lookup_ms[current_shift]) % TIME_24H_MS;
}

uint32_t TimeTrack_ConvertToNonShiftedTime(uint32_t shift_time_ms)
{
    return (shift_time_ms + TIME_24H_MS - timeshift_lookup_ms[current_shift]) % TIME_24H_MS;
}

bool TimeTrack_ShiftToDifferentDay()
{
    uint32_t shifted_time = time_ms + timeshift_lookup_ms[current_shift] % TIME_24H_MS;
    if (current_shift == SHIFT_18 && shifted_time > time_ms)
    {
        return true;
    }
    else if(current_shift == SHIFT_12 && shifted_time > time_ms)
    {
        return true;
    }
    else if(current_shift == SHIFT_6 && shifted_time < time_ms)
    {
        return true;
    }
    return false;
}

uint8_t rtcTimesEqual(RtcTime *time_a, RtcTime *time_b)
{
    if (time_a->sec != time_b->sec)
    {
        return 0;
    }
    else if (time_a->min != time_b->min)
    {
        return 0;
    }
    else if (time_a->hr != time_b->hr)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

uint32_t rtcTimeToMs(RtcTime *time)
{
    return (uint32_t) time->sec * 1000 + (uint32_t) time->min * 60000
            + (uint32_t) time->hr * 3600000;
}

uint32_t gpsTimeToMs(GpsTime *time)
{
    return (uint32_t) time->sec * 1000 + (uint32_t) time->min * 60000
            + (uint32_t) time->hr * 3600000;
}

void msToRtcTime(uint32_t milliseconds, RtcTime *time)
{
    milliseconds = milliseconds / 1000;
    time->sec = milliseconds % 60;
    milliseconds = milliseconds / 60;
    time->min = milliseconds % 60;
    milliseconds = milliseconds / 60;
    time->hr = milliseconds % 24;
}
