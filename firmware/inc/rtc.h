#ifndef FIRMWARE_INC_RTC_H_
#define FIRMWARE_INC_RTC_H_

#include "clock_types.h"

#define MAX_NO_DAYS_RESET   5

typedef enum ALARM_STATUS{
    ALARM_DISABLE,
    ALARM_ENABLE
}ALARM_STATUS;

typedef enum ALARM_ID{
    ALARM = 1,
    TIMER = 2
}ALARM_ID;

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
    void (*setDay)(uint8_t day);
    void (*setMonth)(uint8_t month);
    void (*enableAlarm)(ALARM_ID id, ALARM_STATUS enable);
    void (*setAlarm)(
        ALARM_ID id,
        uint8_t hour_24mode,
        uint8_t minute,
        uint8_t second);

    uint8_t (*getHour)(void);
    uint8_t (*getMinute)(void);
    uint8_t (*getSecond)(void);
    uint8_t (*getDay)(void);
    uint8_t (*getMonth)(void);
    uint8_t (*getAlarmHour)(ALARM_ID id);
    uint8_t (*getAlarmMinute)(ALARM_ID id);
    uint8_t (*getAlarmSecond)(ALARM_ID id);
}Rtc;

ClockStatus Rtc_Init(Rtc *self);
ClockStatus Rtc_IsValid(Rtc *self);
ClockStatus Rtc_SetTime(Rtc *self, RtcTime *time);
ClockStatus Rtc_GetTime(Rtc *self, RtcTime *time);
ClockStatus Rtc_SetAlarm(Rtc *self, RtcTime *time, ALARM_ID id);
ClockStatus Rtc_GetAlarm(Rtc *self, RtcTime *time, ALARM_ID id);
ClockStatus Rtc_EnableAlarm(Rtc *self, ALARM_ID id, ALARM_STATUS enable);

#endif /* FIRMWARE_INC_RTC_H_ */
