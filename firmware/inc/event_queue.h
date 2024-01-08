#ifndef FIRMWARE_INC_EVENT_QUEUE_H_
#define FIRMWARE_INC_EVENT_QUEUE_H_

#include <stdint.h>

#include "clock_types.h"
#include "doz_clock.h"

////////////////////// Defines

#define BUFFER_SIZE     5


////////////////////// Enums

typedef enum btnId
{
    DISPLAY,
    ALARM,
    TIMER,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    DOZ,
    TRAD,
    VOLUP,
    VOLDOWN,
    CANCEL,
    NUM_BUTTON_ID
} BtnId;

typedef enum btnPressType
{
    SHORT,
    LONG,
    NUM_PRESS_TYPES
} BtnPressType;

typedef enum lightEventType
{
    LIGHT_ROOM,
    DARK_ROOM,
    NUM_LIGHT_EVENT_TYPES
} LightEventType;

typedef enum alarmEventType
{
    ALARM_TRIG,
    TIMER_TRIG,
    NUM_ALARM_EVENT_TYPES
} AlarmEventType;

typedef enum EventType
{
    NO_EVENT = -1,
    BTN_EVENT,
    LIGHT_EVENT,
    ALARM_EVENT,
    NUM_EVENT_TYPES
} EventType;


////////////////////// Structs

typedef struct btn_event_type_t
{
    BtnId e_btn_id;
    BtnPressType e_btn_press_type;
} BtnEventType;

typedef struct event_t
{
    EventType e_event_type;
    BtnEventType s_btn_event;
    LightEventType e_light_event;
    AlarmEventType e_alarm_event;
} Event;

typedef struct EventQ
{
    Event s_event;
    struct EventQ *next;
} EventQ;

////////////////////// Functions

ClockStatus EQ_Init();
ClockStatus EQ_GetEvent(Event *event);
ClockStatus EQ_TriggerButtonEvent(BtnId button, BtnPressType type);
ClockStatus EQ_TriggerLightEvent(LightEventType type);
ClockStatus EQ_TriggerAlarmEvent(AlarmEventType type);

#if DEBUG
void printEventQ();
#endif

#endif  // FIRMWARE_INC_EVENT_QUEUE_H_
