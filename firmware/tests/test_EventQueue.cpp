extern "C"
{
#include <string.h>

#include "event_queue.h"
#include "clock_types.h"
}
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

/*
    Mock Functions
*/

/*
    Test Groups
*/

TEST_GROUP(EventQueueModule)
{
    void setup()
    {
    }

    void teardown()
    {
        mock().clear();
    }
};

/*
    Unit Tests
*/

// Display Init Test Group

TEST(EventQueueModule, U21_QueueEmptyRead)
{
    // Production code
    EventId Event;
    EventQ_Init();
    EventQ_GetEvent(&Event);

    // Checks
    CHECK_EQUAL(E_NONE, Event);
}

TEST(EventQueueModule, U22_AddEventWhileQueueEmpty)
{
    // Production code
    EventId Event1, Event2;
    EventQ_Init();
    EventQ_TriggerAlarmEvent(ALARM_TRIG);
    EventQ_GetEvent(&Event1);
    EventQ_GetEvent(&Event2);


    // Checks
    CHECK_EQUAL(E_ALARM_TRIG, Event1);
    CHECK_EQUAL(E_NONE, Event2);
}

TEST(EventQueueModule, U23_AddEventWhileQueueNotEmpty)
{
    // Production code
    EventId Event1, Event2, Event3;
    EventQ_Init();
    EventQ_TriggerLightEvent(LIGHT_ROOM);
    EventQ_TriggerLightEvent(DARK_ROOM);
    EventQ_GetEvent(&Event1);
    EventQ_TriggerAlarmEvent(ALARM_TRIG);
    EventQ_GetEvent(&Event2);
    EventQ_GetEvent(&Event3);

    // Checks
    CHECK_EQUAL(E_ROOM_LIGHT, Event1);
    CHECK_EQUAL(E_ROOM_DARK, Event2);
    CHECK_EQUAL(E_ALARM_TRIG, Event3);
}

TEST(EventQueueModule, U24_AddEventWhileQueueFull)
{
    // Production code
    ClockStatus Status;
    EventQ_Init();
    EventQ_TriggerAlarmEvent(ALARM_TRIG);
    EventQ_TriggerAlarmEvent(ALARM_TRIG);
    EventQ_TriggerAlarmEvent(ALARM_TRIG);
    EventQ_TriggerAlarmEvent(ALARM_TRIG);
    EventQ_TriggerAlarmEvent(ALARM_TRIG);
    Status = EventQ_TriggerAlarmEvent(ALARM_TRIG);

    // Checks
    CHECK_EQUAL(CLOCK_FAIL, Status);
}

TEST(EventQueueModule, U25_FillQueueWithoutReads)
{
    // Production code
    ClockStatus Status;
    EventQ_Init();
    for (unsigned i = 0; i < 5; ++i) {
        EventQ_TriggerButtonEvent(B_DISPLAY, SHORT);
    }
    Status = EventQ_TriggerButtonEvent(B_DISPLAY, LONG);

    // Checks
    CHECK_EQUAL(CLOCK_FAIL, Status);
}
