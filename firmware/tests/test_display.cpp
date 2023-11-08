#include "CppUTest/TestHarness.h"
extern "C"
{
#include "display.h"
#include <string.h>
}

Display display;
Time curr_time;
ClockStatus status;
char output[15];

/*
    Test Groups
*/

TEST_GROUP(DisplayInitTestGroup)
{
    void setup()
    {
        curr_time.format = DEC24H;
        curr_time.milliseconds = 0;
        strcpy(output, "");
        status = CLOCK_OK;
    }
};

TEST_GROUP(Display24hTestGroup)
{
    void setup()
    {
        curr_time.format = DEC24H;
        curr_time.milliseconds = 0;
        strcpy(output, "");
        status = CLOCK_OK;

        Display_Init(&display);
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
    curr_time.milliseconds = 0;
    status = Display_UpdateTime(&display, &curr_time, output);
    STRCMP_EQUAL("00:00:00", output);
    CHECK(status == CLOCK_OK);
}

TEST(Display24hTestGroup, ValidInputNoPadding)
{
    curr_time.milliseconds = 50010000;
    status = Display_UpdateTime(&display, &curr_time, output);
    STRCMP_EQUAL("13:53:30", output);
    CHECK(status == CLOCK_OK);
}

TEST(Display24hTestGroup, ValidInputNoneZeroMs)
{
    curr_time.milliseconds = 50010987;
    status = Display_UpdateTime(&display, &curr_time, output);
    STRCMP_EQUAL("13:53:30", output);
    CHECK(status == CLOCK_OK);
}

TEST(Display24hTestGroup, ValidInputWithPadding)
{
    curr_time.milliseconds = 10809000;
    status = Display_UpdateTime(&display, &curr_time, output);
    STRCMP_EQUAL("03:00:09", output);
    CHECK(status == CLOCK_OK);
}

TEST(Display24hTestGroup, InvalidInput)
{
    curr_time.milliseconds = MILLISECONDS_PER_DAY + 100000;
    status = Display_UpdateTime(&display, &curr_time, output);
    STRCMP_EQUAL("", output);
    CHECK(status != CLOCK_OK);
}

TEST(Display24hTestGroup, BoundaryValue)
{
    curr_time.milliseconds = MILLISECONDS_PER_DAY;
    status = Display_UpdateTime(&display, &curr_time, output);
    STRCMP_EQUAL("", output);
    CHECK(status != CLOCK_OK);
}

TEST(Display24hTestGroup, MaxValue)
{
    curr_time.milliseconds = MILLISECONDS_PER_DAY - 1;
    status = Display_UpdateTime(&display, &curr_time, output);
    STRCMP_EQUAL("23:59:59", output);
    CHECK(status == CLOCK_OK);
}