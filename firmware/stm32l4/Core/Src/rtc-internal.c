/*
 * rtc-internal.c
 *
 *  Created on: Feb 29, 2024
 *      Author: lemck
 */

#include "rtc-internal.h"
#include "event_queue.h"

#define RTC_FORMAT  RTC_FORMAT_BIN

#define STATUS_BACKUP_REG   RTC_BKP_DR1
#define ALARM_BACKUP_REG    RTC_BKP_DR2
#define VALID_STATUS_CODE   0x00FF00FF
#define ALARM_SAVED_CODE    0xDEADBEEF

static RTC_HandleTypeDef *hrtc;
static RTC_TimeTypeDef sTime = {0};
static RTC_DateTypeDef sDate = {0};
static RTC_AlarmTypeDef sAlarm = {0};
static RTC_AlarmTypeDef sTimer = {0};


void RTC_Init(RTC_HandleTypeDef *rtc)
{
    hrtc = rtc;

    HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT);
    HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT);
    HAL_RTC_GetAlarm(hrtc, &sAlarm, sAlarm.Alarm, RTC_FORMAT);
    HAL_RTC_GetAlarm(hrtc, &sTimer, sTimer.Alarm, RTC_FORMAT);

    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;   // What the alarm ignores
    sAlarm.Alarm = RTC_ALARM_A;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;

    sTimer.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;   // What the alarm ignores
    sTimer.Alarm = RTC_ALARM_B;
    sTimer.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTimer.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;

    HAL_RTC_SetTime(hrtc, &sTime, RTC_FORMAT);
    HAL_RTC_SetDate(hrtc, &sDate, RTC_FORMAT);
    HAL_RTC_SetAlarm(hrtc, &sAlarm, RTC_FORMAT);
    HAL_RTC_SetAlarm(hrtc, &sTimer, RTC_FORMAT);

    // Calibration test
    HAL_RTCEx_SetSmoothCalib(hrtc, RTC_SMOOTHCALIB_PERIOD_32SEC, RTC_SMOOTHCALIB_PLUSPULSES_RESET, 0x1F0);
}

void RTC_SetTime(uint8_t hr, uint8_t min, uint8_t sec)
{
    if (hr < 24 && min < 60 && sec < 60)
    {
        sTime.Hours = hr;
        sTime.Minutes = min;
        sTime.Seconds = sec;
        HAL_RTC_SetTime(hrtc, &sTime, RTC_FORMAT);
//        HAL_Delay(2000);
    }
}

void RTC_GetTime(uint8_t *hr, uint8_t *min, uint8_t *sec)
{
    HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT);
    HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT);
    *hr = sTime.Hours;
    *min = sTime.Minutes;
    *sec = sTime.Seconds;
}

void RTC_SetDay(uint8_t d)
{
    if (d <= 31)
    {
        sDate.Date = d;
        HAL_RTC_SetDate(hrtc, &sDate, RTC_FORMAT);
    }
}

uint8_t RTC_GetDay(void)
{
    HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT);
    return sDate.Date;
}

void RTC_SetMonth(uint8_t m)
{
    if (m <= 12)
    {
        sDate.Month = m;
        HAL_RTC_SetDate(hrtc, &sDate, RTC_FORMAT);
    }
}

uint8_t RTC_GetMonth(void)
{
    HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT);
    return sDate.Month;
}

void RTC_SetAlarm(uint8_t id, uint8_t hr, uint8_t min, uint8_t sec)
{
    if (hr < 24 && min < 60 && sec < 60)
    {
        switch (id)
        {
        case ALARM_ID:
            sAlarm.AlarmTime.Hours = hr;
            sAlarm.AlarmTime.Minutes = min;
            sAlarm.AlarmTime.Seconds = sec;
            HAL_RTC_SetAlarm(hrtc, &sAlarm, RTC_FORMAT);
            break;
        case TIMER_ID:
            sTimer.AlarmTime.Hours = hr;
            sTimer.AlarmTime.Minutes = min;
            sTimer.AlarmTime.Seconds = sec;
            HAL_RTC_SetAlarm(hrtc, &sTimer, RTC_FORMAT);
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
    case ALARM_ID:
        HAL_RTC_GetAlarm(hrtc, &sAlarm, sAlarm.Alarm, RTC_FORMAT);
        *hr = sAlarm.AlarmTime.Hours;
        *min = sAlarm.AlarmTime.Minutes;
        *sec = sAlarm.AlarmTime.Seconds;
        break;
    case TIMER_ID:
        HAL_RTC_GetAlarm(hrtc, &sTimer, sTimer.Alarm, RTC_FORMAT);
        *hr = sTimer.AlarmTime.Hours;
        *min = sTimer.AlarmTime.Minutes;
        *sec = sTimer.AlarmTime.Seconds;
        break;
    default:
        break;
    }
}

void RTC_EnableAlarm(uint8_t id, bool enable)
{
    switch (id)
    {
    case ALARM_ID:
        if (enable)
        {
            // Write valid save status for alarm
            HAL_RTCEx_BKUPWrite(hrtc, ALARM_BACKUP_REG, ALARM_SAVED_CODE);
            HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, RTC_FORMAT);
        }
        else
        {
            // Remove save status for alarm
            HAL_RTCEx_BKUPWrite(hrtc, ALARM_BACKUP_REG, 0x0);
            HAL_RTC_DeactivateAlarm(hrtc, sAlarm.Alarm);
        }
        break;
    case TIMER_ID:
        if (enable)
        {
            HAL_RTC_SetAlarm_IT(hrtc, &sTimer, RTC_FORMAT);
        }
        else
        {
            HAL_RTC_DeactivateAlarm(hrtc, sTimer.Alarm);
        }
        break;
    default:
        break;
    }
}

bool RTC_CheckDataSaved(RTC_HandleTypeDef *rtc)
{
    // STATUS_BACKUP_REG is reset to 0x0 if backup battery power is lost
    if (HAL_RTCEx_BKUPRead(rtc, STATUS_BACKUP_REG) != VALID_STATUS_CODE)
    {
        // Set STATUS_BACKUP_REG to VALID_STATUS_CODE
        HAL_RTCEx_BKUPWrite(rtc, STATUS_BACKUP_REG, VALID_STATUS_CODE);
        return false;
    }
    return true;
}

bool RTC_CheckAlarmSaved(uint8_t id)
{
    // ALARM_BACKUP_REG is reset to 0x0 if backup battery power is lost
    if (id == ALARM_ID && HAL_RTCEx_BKUPRead(hrtc, ALARM_BACKUP_REG) == ALARM_SAVED_CODE)
    {
        // Power wasn't lost an saved alarm value is valid
        return true;
    }
    return false;
}


void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *rtc)
{
    if (rtc == hrtc)
    {
        EventQ_TriggerAlarmEvent(ALARM_TRIG);
    }
}

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *rtc)
{
    if (rtc == hrtc)
    {
        EventQ_TriggerAlarmEvent(TIMER_TRIG);
    }
}
