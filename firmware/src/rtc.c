#include "rtc.h"

#include <stdio.h>
#include <string.h>

ClockStatus Rtc_Init(Rtc *self)
{
    return CLOCK_OK;
}

ClockStatus Rtc_SetTime(Rtc *self, RtcTime *time)
{
    if (self->setRtcTime != NULL)
    {
        self->setRtcTime(
            time->hr,
            time->min,
            time->sec,
            time->date,
            time->month,
            time->dow,
            time->year);
    }
    return CLOCK_OK;
}

ClockStatus Rtc_GetTime(Rtc *self, RtcTime *time)
{
    if (self->getDayOfWeek != NULL)     { time->dow = self->getDayOfWeek(); }
    if (self->getDate != NULL)          { time->date = self->getDate(); }
    if (self->getMonth != NULL)         { time->month = self->getMonth(); }
    if (self->getYear != NULL)          { time->year = self->getYear(); }
    if (self->getHour != NULL)          { time->hr = self->getHour(); }
    if (self->getMinute != NULL)        { time->min = self->getMinute(); }
    if (self->getSecond != NULL)        { time->sec = self->getSecond(); }
    return CLOCK_OK;
}
