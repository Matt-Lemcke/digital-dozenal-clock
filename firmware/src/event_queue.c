#include "event_queue.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static BtnEventType BUTTON_EVENT_ARRAY[NUM_PRESS_TYPES][NUM_BUTTON_ID];
static EventQ *head;

static void bufferPush(Event event);
static void bufferPop();
static bool bufferFull();
static bool bufferEmpty();

ClockStatus EQ_Init()
{
    BtnEventType btnEventType;
    for (unsigned pressType = 0; pressType < NUM_PRESS_TYPES; ++pressType) {
        for (unsigned btnId = 0; btnId < NUM_BUTTON_ID; ++btnId) {
            btnEventType.e_btn_id = btnId;
            btnEventType.e_btn_press_type = pressType;
            BUTTON_EVENT_ARRAY[pressType][btnId] = btnEventType;
        }
    }

    head = NULL;

    return CLOCK_OK;
}

ClockStatus EQ_GetEvent(Event *event)
{
    if (bufferEmpty()) {
        return CLOCK_FAIL;
    }

    memcpy(event, &head->s_event, sizeof(*event));
    bufferPop();

    return CLOCK_OK;
}

ClockStatus EQ_TriggerButtonEvent(BtnId button, BtnPressType type)
{
    Event event = { .e_event_type = BTN_EVENT,
                    .s_btn_event = BUTTON_EVENT_ARRAY[type][button],
                    .e_light_event = 0,
                    .e_alarm_event = 0 };
    bufferPush(event);
    return CLOCK_OK;
}

ClockStatus EQ_TriggerLightEvent(LightEventType type)
{
    Event event = { .e_event_type = LIGHT_EVENT,
                    .s_btn_event = { 0 },
                    .e_light_event = type,
                    .e_alarm_event = 0 };
    bufferPush(event);
    return CLOCK_OK;
}

ClockStatus EQ_TriggerAlarmEvent(AlarmEventType type)
{
    Event event = { .e_event_type = ALARM_EVENT,
                    .s_btn_event = { 0 },
                    .e_light_event = 0,
                    .e_alarm_event = type };
    bufferPush(event);
    return CLOCK_OK;
}

static void bufferPush(Event event)
{
    if (bufferFull()) {
        return;
    }

    EventQ *entry = malloc(sizeof(*entry));
    entry->s_event = event;
    entry->next = NULL;

    if (head == NULL) {
        head = entry;
        return;
    }

    EventQ *temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = entry;
}

static void bufferPop()
{
    EventQ *temp = head;
    if (head->next != NULL) {
        head = head->next;
    } else {
        head = NULL;
    }
    free(temp);
}

static bool bufferFull()
{
    if (bufferEmpty()) {
        return false;
    }

    unsigned buffCount = 1;
    EventQ *temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
        ++buffCount;
    }

    return (buffCount >= BUFFER_SIZE);
}

static bool bufferEmpty()
{
    return (head == NULL);
}


#if DEBUG
void printEventQ()
{
    printf("--------------- EQ PRINT START\n\n");
    if (head == NULL) {
        printf("Buffer is Empty\n");
        printf("\n--------------- EQ PRINT END\n\n");
        return;
    }

    EventQ *temp = head;
    unsigned i = 1;
    do {
        printf("\nEvent %u -----------\n", i++);
        printf("Event Type: %d\nBtn Event: %d %d\n",
                temp->s_event.e_event_type,
                temp->s_event.s_btn_event.e_btn_id,
                temp->s_event.s_btn_event.e_btn_press_type);
        printf("Light Event: %d\nAlarm Event: %d\n",
                temp->s_event.e_light_event,
                temp->s_event.e_alarm_event);

        temp = temp->next;
    } while (temp != NULL);

    printf("\n--------------- EQ PRINT END\n\n");
}
#endif
