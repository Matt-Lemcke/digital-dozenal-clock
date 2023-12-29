#ifndef FIRMWARE_INC_RTC_H_
#define FIRMWARE_INC_RTC_H_

#include <stdint.h>

#include "clock_types.h"

typedef struct rtc_time_t
{
    uint8_t hr;
    uint8_t min;
    uint8_t sec;
} RtcTime;

typedef struct rtc_t
{
    void (*setRtcTime)(
        uint8_t hour_24mode,
        uint8_t minute,
        uint8_t second);
    uint8_t (*getHour)(void);
    uint8_t (*getMinute)(void);
    uint8_t (*getSecond)(void);
}Rtc;

ClockStatus Rtc_Init(Rtc *self);
ClockStatus Rtc_SetTime(Rtc *self, RtcTime *time);
ClockStatus Rtc_GetTime(Rtc *self, RtcTime *time);

#endif /* FIRMWARE_INC_RTC_H_ */
