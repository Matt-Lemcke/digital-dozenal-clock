#include <rtc_module.h>

static Rtc *g_rtc;

ClockStatus Rtc_Init(Rtc *self)
{
    g_rtc = self;
    if (self->getTime == NULL)
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
    if (g_rtc->getTime == NULL) {
        return CLOCK_FAIL;
    }
    g_rtc->getTime(&time->hr, &time->min, &time->sec);

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
    if (g_rtc->getAlarm == NULL) {
        return CLOCK_FAIL;
    }

    g_rtc->getAlarm(id, &time->hr, &time->min, &time->sec);

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

ClockStatus Rtc_GetAlarmStatus(AlarmId id, bool *status)
{
    if (g_rtc->getAlarmStatus == NULL) {
        return CLOCK_FAIL;
    }

    *status = g_rtc->getAlarmStatus(id);

    return CLOCK_OK;
}

ClockStatus Rtc_SetCalibration(int32_t val)
{
    if (g_rtc->setCalibration == NULL || g_rtc->setCalibration(val) == 0)
    {
        return CLOCK_FAIL;
    }
    return CLOCK_OK;
}

ClockStatus Rtc_GetCalibration(int32_t *val)
{
    if (g_rtc->getCalibration == NULL)
    {
        *val = 0;
        return CLOCK_FAIL;
    }
    *val = g_rtc->getCalibration();
    return CLOCK_OK;
}
