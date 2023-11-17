extern "C"
{
#include <string.h>

#include "display.h"
}
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

Display display;
ClockStatus status;
TimeFormat format;

/*
    Mock Functions
*/
void clearDisplay()
{
    mock().actualCall("clearDisplay");
}
void displayString(char *st)
{
    mock().actualCall("displayString").withParameter("st", st);
}

/*
    Test Groups
*/

TEST_GROUP(DisplayInitTestGroup)
{
    void setup()
    {
        status = CLOCK_OK;
    }
};

TEST_GROUP(Display24hTestGroup)
{
    void setup()
    {
        format = DEC24H;
        status = CLOCK_OK;

        display.clear = clearDisplay;
        display.sendString = displayString;
        Display_Init(&display);
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

TEST(DisplayInitTestGroup, DefaultValues)
{
    Display_Init(&display);
    CHECK_EQUAL(100, display.brightness);
}

// Display 24h Test Group

TEST(Display24hTestGroup, ZeroInput)
{
    mock().expectOneCall("clearDisplay");
    mock().expectOneCall("displayString").withParameter("st", "00:00:00");
    status = Display_UpdateTime(&display, 0, format);
    CHECK(status == CLOCK_OK);
    mock().checkExpectations();
}

TEST(Display24hTestGroup, ValidInputNoPadding)
{
    mock().expectOneCall("clearDisplay");
    mock().expectOneCall("displayString").withParameter("st", "13:53:30");
    status = Display_UpdateTime(&display, 50010000, format);
    CHECK(status == CLOCK_OK);
}

TEST(Display24hTestGroup, ValidInputNoneZeroMs)
{
    mock().expectOneCall("clearDisplay");
    mock().expectOneCall("displayString").withParameter("st", "13:53:30");
    status = Display_UpdateTime(&display, 50010987, format);
    CHECK(status == CLOCK_OK);
}

TEST(Display24hTestGroup, ValidInputWithPadding)
{
    mock().expectOneCall("clearDisplay");
    mock().expectOneCall("displayString").withParameter("st", "03:00:09");
    status = Display_UpdateTime(&display, 10809000, format);
    CHECK(status == CLOCK_OK);
}

TEST(Display24hTestGroup, InvalidInput)
{
    mock().expectOneCall("clearDisplay");
    mock().expectOneCall("displayString").withParameter("st", "");
    status = Display_UpdateTime(&display, MILLISECONDS_PER_DAY + 100000, format);
    CHECK(status != CLOCK_OK);
}

TEST(Display24hTestGroup, BoundaryValue)
{
    mock().expectOneCall("clearDisplay");
    mock().expectOneCall("displayString").withParameter("st", "");
    status = Display_UpdateTime(&display, MILLISECONDS_PER_DAY, format);
    CHECK(status != CLOCK_OK);
}

TEST(Display24hTestGroup, MaxValue)
{
    mock().expectOneCall("clearDisplay");
    mock().expectOneCall("displayString").withParameter("st", "23:59:59");
    status = Display_UpdateTime(&display,  MILLISECONDS_PER_DAY - 1, format);
    CHECK(status == CLOCK_OK);
}
