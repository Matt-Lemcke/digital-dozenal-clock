extern "C"
{
#include <string.h>

#include "display.h"
}
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

Display display;
ClockStatus status;
ExternVars vars;

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

TEST(DisplayInitTestGroup, PlaceholderTest)
{
    status = Display_Init(&display, &vars);
    CHECK_EQUAL(status, CLOCK_FAIL);
}

