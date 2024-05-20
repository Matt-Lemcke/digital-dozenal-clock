#ifndef FIRMWARE_INC_RTC_H_
#define FIRMWARE_INC_RTC_H_

#include "clock_types.h"

#define MAX_NO_DAYS_RESET   5

typedef enum AlarmStatus{
    ALARM_DISABLE   = 0,
    ALARM_ENABLE    = 1,
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
    void (*getTime)(
        uint8_t *hour_24mode,
        uint8_t *minute,
        uint8_t *second);
    void (*setDay)(uint8_t day);
    void (*setMonth)(uint8_t month);
    uint8_t (*getDay)(void);
    uint8_t (*getMonth)(void);
    void (*setAlarm)(
        uint8_t id,
        uint8_t hour_24mode,
        uint8_t minute,
        uint8_t second);
    void (*getAlarm)(
        uint8_t id,
        uint8_t *hour_24mode,
        uint8_t *min,
        uint8_t *sec);
    void (*enableAlarm)(uint8_t id, bool enable);
    bool (*getAlarmStatus)(uint8_t id);
    bool (*setCalibration)(int32_t calib);
    int32_t (*getCalibration)(void);
    int32_t max_calib;

}Rtc;

ClockStatus Rtc_Init(Rtc *self);
ClockStatus Rtc_IsValid();
ClockStatus Rtc_SetTime(RtcTime *time);
ClockStatus Rtc_GetTime(RtcTime *time);
ClockStatus Rtc_SetAlarm(RtcTime *time, AlarmId id);
ClockStatus Rtc_GetAlarm(RtcTime *time, AlarmId id);
ClockStatus Rtc_EnableAlarm(AlarmId id, AlarmStatus enable);
ClockStatus Rtc_GetAlarmStatus(AlarmId id, bool *status);
ClockStatus Rtc_SetCalibration(int32_t val);
ClockStatus Rtc_GetCalibration(int32_t *val);

#endif /* FIRMWARE_INC_RTC_H_ */
