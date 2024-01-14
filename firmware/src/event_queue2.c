#include "event_queue2.h"

#include <stdio.h>

static struct {
    EventId buffer[BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
    bool is_empty;
}ring_buff;

static void bufferPush(EventId event);
static EventId bufferPop(void);
static bool bufferFull(void);
static bool bufferEmpty(void);

/*
 *  Public functions
*/
ClockStatus EventQ_Init()
{
    // < No potential reference to NULL pointer since these are just indexing an array >
    ring_buff.head = 0;
    ring_buff.tail = 0;
    ring_buff.is_empty = 1;
}

ClockStatus EventQ_GetEvent(EventId *event)
{
    if (bufferEmpty())
    {
        *event = E_NONE;
        return CLOCK_FAIL;
    }
    *event = bufferPop();
    return CLOCK_OK;
}

ClockStatus EventQ_TriggerButtonEvent(BtnId button, BtnPressType type)
{
    EventId btn_event = E_NONE;
    if (bufferFull()) 
    {
        return CLOCK_FAIL;
    }
    switch (button)
    {
        case DISPLAY:
            btn_event = (type == LONG) ? E_DISPLAY_LONG : E_DISPLAY_SHORT;
            break;
        case ALARM:
            btn_event = (type == LONG) ? E_ALARM_LONG : E_ALARM_SHORT;
            break;
        case TIMER:
            btn_event = (type == LONG) ? E_TIMER_LONG : E_TIMER_SHORT;
            break;
        case LEFT:
            btn_event = (type == LONG) ? E_LEFT_LONG : E_LEFT_SHORT;
            break;
        case RIGHT:
            btn_event = (type == LONG) ? E_RIGHT_LONG : E_RIGHT_SHORT;
            break;
        case UP:
            btn_event = (type == LONG) ? E_UP_LONG : E_UP_SHORT;
            break;
        case DOWN:
            btn_event = (type == LONG) ? E_DOWN_LONG : E_DOWN_SHORT;
            break;
        case DOZ:
            btn_event = (type == LONG) ? E_DOZ_LONG : E_DOZ_SHORT;
            break;
        case TRAD:
            btn_event = (type == LONG) ? E_TRAD_LONG : E_TRAD_SHORT;
            break;
        case VOLUP:
            btn_event = (type == LONG) ? E_VOLUP_LONG : E_VOLUP_SHORT;
            break;
        case VOLDOWN:
            btn_event = (type == LONG) ? E_VOLDOWN_LONG : E_VOLDOWN_SHORT;
            break;
        case CANCEL:
            btn_event = (type == LONG) ? E_CANCEL_LONG : E_CANCEL_SHORT;
            break;
        default:
            break;
    }
    if (btn_event == E_NONE)
    {
        return CLOCK_FAIL;
    }
    else
    {
        bufferPush(btn_event);
        return CLOCK_OK;
    }
}

ClockStatus EventQ_TriggerLightEvent(LightEventType type)
{
    if (bufferFull()) 
    {
        return CLOCK_FAIL;
    }
    switch (type)
    {
        case LIGHT_ROOM:
            bufferPush(E_ROOM_LIGHT);
            break;
        case DARK_ROOM:
            bufferPush(E_ROOM_DARK);
            break;
        default:
            return CLOCK_FAIL;
    }
    return CLOCK_OK;
}

ClockStatus EventQ_TriggerAlarmEvent(AlarmEventType type)
{
    if (bufferFull()) 
    {
        return CLOCK_FAIL;
    }
    switch (type)
    {
        case ALARM_TRIG:
            bufferPush(E_ALARM_TRIG);
            break;
        case TIMER_TRIG:
            bufferPush(E_TIMER_TRIG);
            break;
        default:
            return CLOCK_FAIL;
    }
    return CLOCK_OK;
}

/*
 *  Private functions
*/
void bufferPush(EventId event)
{
    // < No use of dynamic memory allocation since array is already created in static memory >
    if (!bufferFull())
    {
        ring_buff.buffer[ring_buff.tail] = event;
        ring_buff.is_empty = 0;
        ring_buff.tail = (ring_buff.tail + 1) % BUFFER_SIZE;
    }
}

EventId bufferPop(void)
{
    // < No use of dynamic memory freeing since array is already created in static memory >
    EventId event = E_NONE;
    if (!bufferEmpty())
    {
        event = ring_buff.buffer[ring_buff.head];
        ring_buff.head = (ring_buff.head + 1) % BUFFER_SIZE;
        if(ring_buff.head == ring_buff.tail)
        {
            ring_buff.is_empty = 1;
        }
    }
    return event;
}

bool bufferFull(void)
{
    // < No need to iterate through buffer >
    return !ring_buff.is_empty && (ring_buff.head == ring_buff.tail);
}

bool bufferEmpty(void)
{
    return ring_buff.is_empty && (ring_buff.head == ring_buff.tail);
}
