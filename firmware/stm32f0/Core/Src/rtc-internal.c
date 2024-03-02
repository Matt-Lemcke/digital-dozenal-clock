/*
 * rtc-internal.c
 *
 *  Created on: Feb 29, 2024
 *      Author: lemck
 */

#include "rtc-internal.h"
#include "event_queue.h"

#define RTC_FORMAT  RTC_FORMAT_BIN

static RTC_HandleTypeDef *hrtc;
static RTC_TimeTypeDef sTime = {0};
static RTC_DateTypeDef sDate = {0};
static RTC_AlarmTypeDef sAlarm = {0};


void RTC_Init(RTC_HandleTypeDef *rtc)
{
    hrtc = rtc;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    sAlarm.AlarmMask = RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES | RTC_ALARMMASK_SECONDS;
    sAlarm.Alarm = RTC_ALARM_A;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;

    HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT);
    HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT);
    HAL_RTC_GetAlarm(hrtc, &sAlarm, RTC_FORMAT);
}

void RTC_SetTime(uint8_t hr, uint8_t min, uint8_t sec)
{
    if (hr < 24 && min < 60 && sec < 60)
    {
        sTime.Hours = hr;
        sTime.Minutes = min;
        sTime.Seconds = sec;
        HAL_RTC_SetTime(hrtc, &sTime, RTC_FORMAT);
    }
}

void RTC_GetTime(uint8_t *hr, uint8_t *min, uint8_t *sec)
{
    HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT);
    hr = &sTime.Hours;
    min = &sTime.Minutes;
    sec = &sTime.Seconds;
}

void RTC_SetDay(uint8_t d)
{
    if (d <= 31)
    {
        sDate.Date = d;
        HAL_RTC_SetDate(hrtc, &sDate, RTC_FORMAT);
    }
}

void RTC_GetDay(uint8_t *d)
{
    HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT);
    d = &sDate.Date;
}

void RTC_SetMonth(uint8_t m)
{
    if (d <= 12)
    {
        sDate.Month = m;
        HAL_RTC_SetDate(hrtc, &sDate, RTC_FORMAT);
    }
}

void RTC_GetMonth(uint8_t *m)
{
    HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT);
    m = &sDate.Month;
}

void RTC_SetAlarm(uint8_t id, uint8_t hr, uint8_t min, uint8_t sec)
{
    if (hr < 24 && min < 60 && sec < 60)
    {
        switch (id)
        {
        case ALARM1_ID:
            sAlarm.AlarmTime.Hours = hr;
            sAlarm.AlarmTime.Minutes = min;
            sAlarm.AlarmTime.Seconds = sec;
            HAL_RTC_SetAlarm(hrtc, &sAlarm, RTC_FORMAT);
            break;
        case ALARM2_ID:
            break;
        default:
            break;
        }
    }
}

void RTC_GetAlarm(uint8_t id, uint8_t *hr, uint8_t *min, uint8_t *sec)
{
    switch (id)
    {
    case ALARM1_ID:
        HAL_RTC_GetAlarm(hrtc, &sAlarm, RTC_FORMAT);
        hr = &sAlarm.AlarmTime.Hours;
        min = &sAlarm.AlarmTime.Minutes;
        sec = &sAlarm.AlarmTime.Seconds;
        break;
    case ALARM2_ID:
        break;
    default:
        break;
    }
}

void RTC_EnableAlarm(uint8_t id, bool enable)
{
    switch (id)
    {
    case ALARM1_ID:
        if (enable)
        {
            HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, RTC_FORMAT);
        }
        else
        {
            HAL_RTC_DeactivateAlarm(hrtc, sAlarm.Alarm);
        }
        break;
    case ALARM2_ID:
        break;
    default:
        break;
    }
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *rtc)
{
    if (rtc == hrtc)
    {
        EventQ_TriggerAlarmEvent(ALARM_TRIG);
    }
}
