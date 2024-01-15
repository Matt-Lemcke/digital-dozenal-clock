#ifndef FIRMWARE_INC_RTC_H_
#define FIRMWARE_INC_RTC_H_

#include "clock_types.h"

#define MAX_NO_DAYS_RESET   5

typedef enum AlarmStatus{
    ALARM_DISABLE,
    ALARM_ENABLE
}AlarmStatus;

typedef enum AlarmId{
    ALARM = 1,
    TIMER = 2
}AlarmId;

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
    void (*enableAlarm)(uint8_t id, bool enable);
    void (*setAlarm)(
        uint8_t id,
        uint8_t hour_24mode,
        uint8_t minute,
        uint8_t second);

    uint8_t (*getHour)(void);
    uint8_t (*getMinute)(void);
    uint8_t (*getSecond)(void);
    uint8_t (*getDay)(void);
    uint8_t (*getMonth)(void);
    uint8_t (*getAlarmHour)(uint8_t id);
    uint8_t (*getAlarmMinute)(uint8_t id);
    uint8_t (*getAlarmSecond)(uint8_t id);
}Rtc;

ClockStatus Rtc_Init(Rtc *self);
ClockStatus Rtc_IsValid(Rtc *self);
ClockStatus Rtc_SetTime(Rtc *self, RtcTime *time);
ClockStatus Rtc_GetTime(Rtc *self, RtcTime *time);
ClockStatus Rtc_SetAlarm(Rtc *self, RtcTime *time, AlarmId id);
ClockStatus Rtc_GetAlarm(Rtc *self, RtcTime *time, AlarmId id);
ClockStatus Rtc_EnableAlarm(Rtc *self, AlarmId id, AlarmStatus enable);

#endif /* FIRMWARE_INC_RTC_H_ */
