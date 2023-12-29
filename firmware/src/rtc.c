#include "rtc.h"

#include <stdio.h>
#include <string.h>

ClockStatus Rtc_Init(Rtc *self)
{
    if (self->getHour != NULL && self->getHour()) {
        return CLOCK_OK;
    }
    return CLOCK_FAIL;
}

ClockStatus Rtc_SetTime(Rtc *self, RtcTime *time)
{
    if (self->setRtcTime == NULL) {
        return CLOCK_FAIL;
    }

    self->setRtcTime(
            time->hr,
            time->min,
            time->sec);

    if (self->setDay == NULL || self->setMonth == NULL) {
        return CLOCK_FAIL;
    }

    self->setDay(1);
    self->setMonth(1);

    return CLOCK_OK;
}

ClockStatus Rtc_GetTime(Rtc *self, RtcTime *time)
{
    if (self->getHour == NULL
        || self->getMinute == NULL
        || self->getSecond == NULL) {
        return CLOCK_FAIL;
    }

    time->hr = self->getHour();
    time->min = self->getMinute();
    time->sec = self->getSecond();

    return CLOCK_OK;
}

ClockStatus Rtc_IsValid(Rtc *self)
{
    if (self->getDay == NULL || self->getMonth == NULL) {
        return CLOCK_FAIL;
    }

    if (self->getDay() > MAX_NO_DAYS_RESET || self->getMonth() > 1) {
        return CLOCK_FAIL;
    }
    return CLOCK_OK;
}

ClockStatus Rtc_SetAlarm(Rtc *self, RtcTime *time, ALARM_ID id)
{
    if ((id != ALARM && id != TIMER) || self->setAlarm == NULL) {
        return CLOCK_FAIL;
    }

    self->setAlarm(id, time->hr, time->min, time->sec);

    return CLOCK_OK;
}

ClockStatus Rtc_GetAlarm(Rtc *self, RtcTime *time, ALARM_ID id)
{
    if ((id != ALARM && id != TIMER)
            || self->getAlarmHour == NULL
            || self->getAlarmMinute == NULL
            || self->getAlarmSecond == NULL) {
        return CLOCK_FAIL;
    }

    time->hr = self->getAlarmHour(id);
    time->min = self->getAlarmMinute(id);
    time->sec = self->getAlarmSecond(id);

    return CLOCK_OK;
}

ClockStatus Rtc_EnableAlarm(Rtc *self, ALARM_ID id, ALARM_STATUS enable)
{
    if ((id != ALARM && id != TIMER) || self->enableAlarm == NULL) {
        return CLOCK_FAIL;
    }

    self->enableAlarm(id, enable);

    return CLOCK_OK;
}
