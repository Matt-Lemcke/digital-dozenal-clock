#include "rtc_module.h"

Rtc *g_rtc;

ClockStatus Rtc_Init(Rtc *self)
{
    g_rtc = self;
    if (self->getHour == NULL)
    {
        return CLOCK_FAIL;
    }
    return CLOCK_OK;
}

ClockStatus Rtc_SetTime(RtcTime *time)
{
    if (g_rtc->setRtcTime == NULL) {
        return CLOCK_FAIL;
    }

    g_rtc->setRtcTime(
            time->hr,
            time->min,
            time->sec);

    if (g_rtc->setDay == NULL || g_rtc->setMonth == NULL) {
        return CLOCK_FAIL;
    }

    g_rtc->setDay(1);
    g_rtc->setMonth(1);

    return CLOCK_OK;
}

ClockStatus Rtc_GetTime(RtcTime *time)
{
    if (g_rtc->getHour == NULL
        || g_rtc->getMinute == NULL
        || g_rtc->getSecond == NULL) {
        return CLOCK_FAIL;
    }

    time->hr = g_rtc->getHour();
    time->min = g_rtc->getMinute();
    time->sec = g_rtc->getSecond();

    return CLOCK_OK;
}

ClockStatus Rtc_IsValid()
{
    if (g_rtc->getDay == NULL || g_rtc->getMonth == NULL) {
        return CLOCK_FAIL;
    }

    if (g_rtc->getDay() > MAX_NO_DAYS_RESET || g_rtc->getMonth() > 1) {
        return CLOCK_FAIL;
    }
    return CLOCK_OK;
}

ClockStatus Rtc_SetAlarm(RtcTime *time, AlarmId id)
{
    if (g_rtc->setAlarm == NULL) {
        return CLOCK_FAIL;
    }

    g_rtc->setAlarm(id, time->hr, time->min, time->sec);

    return CLOCK_OK;
}

ClockStatus Rtc_GetAlarm(RtcTime *time, AlarmId id)
{
    if (g_rtc->getAlarmHour == NULL
        || g_rtc->getAlarmMinute == NULL
        || g_rtc->getAlarmSecond == NULL) {
        return CLOCK_FAIL;
    }

    time->hr = g_rtc->getAlarmHour(id);
    time->min = g_rtc->getAlarmMinute(id);
    time->sec = g_rtc->getAlarmSecond(id);

    return CLOCK_OK;
}

ClockStatus Rtc_EnableAlarm(AlarmId id, AlarmStatus enable)
{
    if (g_rtc->enableAlarm == NULL) {
        return CLOCK_FAIL;
    }

    g_rtc->enableAlarm(id, enable);

    return CLOCK_OK;
}
