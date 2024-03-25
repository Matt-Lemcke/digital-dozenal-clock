#ifndef FIRMWARE_INC_EVENT_QUEUE_H_
#define FIRMWARE_INC_EVENT_QUEUE_H_

#include "clock_types.h"

#define BUFFER_SIZE     5

typedef enum btnId
{
    B_DISPLAY,
    B_DOZ,
    B_TRAD,
    B_LEFT,
    B_RIGHT,
    B_UP,
    B_DOWN,
    B_ALARM,
    B_TIMER,
    B_VOLUP,
    B_VOLDOWN,
    B_CANCEL
} BtnId;

typedef enum btnPressType
{
    SHORT,
    LONG
} BtnPressType;

typedef enum lightEventType
{
    LIGHT_ROOM,
    DARK_ROOM
} LightEventType;

typedef enum alarmEventType
{
    ALARM_TRIG,
    TIMER_TRIG
} AlarmEventType;

typedef enum eventIdType
{
    E_NONE,

    // Alarm events
    E_ALARM_TRIG,
    E_TIMER_TRIG,

    // Light events
    E_ROOM_DARK,
    E_ROOM_LIGHT,

    // Long btn events
    E_DISPLAY_LONG,
    E_ALARM_LONG,
    E_TIMER_LONG,
    E_LEFT_LONG,
    E_RIGHT_LONG,
    E_UP_LONG,
    E_DOWN_LONG,
    E_DOZ_LONG,
    E_TRAD_LONG,
    E_VOLUP_LONG,
    E_VOLDOWN_LONG,
    E_CANCEL_LONG,

    // Short btn events
    E_DISPLAY_SHORT,
    E_ALARM_SHORT,
    E_TIMER_SHORT,
    E_LEFT_SHORT,
    E_RIGHT_SHORT,
    E_UP_SHORT,
    E_DOWN_SHORT,
    E_DOZ_SHORT,
    E_TRAD_SHORT,
    E_VOLUP_SHORT,
    E_VOLDOWN_SHORT,
    E_CANCEL_SHORT,

    NUM_EVENTS
} EventId;

ClockStatus EventQ_Init();
ClockStatus EventQ_GetEvent(EventId *event);
ClockStatus EventQ_TriggerButtonEvent(BtnId button, BtnPressType type);
ClockStatus EventQ_TriggerLightEvent(LightEventType type);
ClockStatus EventQ_TriggerAlarmEvent(AlarmEventType type);



#endif  // FIRMWARE_INC_EVENT_QUEUE_H_
