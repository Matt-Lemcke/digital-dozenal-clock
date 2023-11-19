#ifndef FIRMWARE_INC_RTC_H_
#define FIRMWARE_INC_RTC_H_

#include <stdint.h>

#include "clock_types.h"

typedef struct rtc_time_t
{
    uint8_t hr;
    uint8_t min;
    uint8_t sec;
    uint8_t date;
    uint8_t month;
    uint8_t dow;
    uint16_t year;
} RtcTime;

typedef struct rtc_t
{
    void (*setRtcTime)(
        uint8_t hour_24mode,
        uint8_t minute,
        uint8_t second,
        uint8_t date,
        uint8_t month,
        uint8_t dow,
        uint16_t year);
    uint8_t (*getDayOfWeek)(void);
    uint8_t (*getDate)(void);
    uint8_t (*getMonth)(void);
    uint16_t (*getYear)(void);
    uint8_t (*getHour)(void);
    uint8_t (*getMinute)(void);
    uint8_t (*getSecond)(void);
}Rtc;

ClockStatus Rtc_Init(Rtc *self);
ClockStatus Rtc_SetTime(Rtc *self, RtcTime *time);
ClockStatus Rtc_GetTime(Rtc *self, RtcTime *time);

#endif /* FIRMWARE_INC_RTC_H_ */
