extern "C"
{
#include <string.h>

#include "display.h"
#include "clock_types.h"
#include "doz_clock.h"
}
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

Display testDisplay;
ExternVars testExternVars;
uint8_t * mockBitmap;

/*
    Mock Functions
*/
void setBrightness(uint8_t brightness)
{
    mock().actualCall("setBrightness").withParameter("brightness", brightness);   
}

void setColour(uint8_t region_id, Colour colour_id)
{
    mock().actualCall("setColour").withParameter("region_id", region_id).withParameter("colour_id", colour_id);   
}

void setBitmap(uint8_t region_id, uint8_t *bitmap)
{
    mock().actualCall("setBitmap").withParameter("region_id", region_id).withParameter("bitmap", bitmap);   
}

void displayOn(void)
{
    mock().actualCall("displayOn");   
}

void displayOff(void)
{
    mock().actualCall("displayOff");   
}

void show(uint8_t region_id)
{
    mock().actualCall("show").withParameter("region_id", region_id);   
}

void hide(uint8_t region_id)
{
    mock().actualCall("hide").withParameter("region_id", region_id);   
}

/*
    Test Groups
*/

TEST_GROUP(DisplayModule)
{
    void setup()
    {
        testDisplay.setBrightness = setBrightness;
        testDisplay.setColour = setColour;
        testDisplay.setBitmap = setBitmap;
        testDisplay.displayOn = displayOn;
        testDisplay.displayOff = displayOff;
        testDisplay.show = show;
        testDisplay.hide = hide;
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

TEST(DisplayModule, U31_TimeFormatChangingStates)
{
    // Setup mock function calls

    // Display Init
    mock().expectNCalls(1,"setBrightness").ignoreOtherParameters();
    mock().expectNCalls(3,"setColour").ignoreOtherParameters();
    mock().expectNCalls(3,"setBitmap").ignoreOtherParameters();
    mock().expectNCalls(1,"displayOff");
    // Display Init

    mock().expectNCalls(1,"displayOn");
    mock().expectNCalls(11,"show").ignoreOtherParameters();
    mock().expectNCalls(1,"hide").ignoreOtherParameters();

    // Production code
    TimeFormats Format1, Format2, Format3;
    Display_Init(&testDisplay, &testExternVars);
    Display_On();

    Display_SetFormat(DOZ_DRN4);
    Display_SetTime();
    Format1 = testDisplay.time_format;
    Display_ShowTime();
    Format2 = testDisplay.time_format;
    Display_SetFormat(DOZ_SEMI);
    Display_ToggleMode();
    Format3 = testDisplay.time_format;

    // Checks
    CHECK_EQUAL(DOZ_DRN4, Format1);
    CHECK_EQUAL(DOZ_DRN4, Format2);
    CHECK_EQUAL(DOZ_SEMI, Format3);
}

TEST(DisplayModule, U32_DisplayOffWhileSetAlarm)
{
    // Setup mock function calls

    // Display Init
    mock().expectNCalls(1,"setBrightness").ignoreOtherParameters();
    mock().expectNCalls(3,"setColour").ignoreOtherParameters();
    mock().expectNCalls(3,"setBitmap").ignoreOtherParameters();
    mock().expectNCalls(1,"displayOff");
    // Display Init

    mock().expectNCalls(1,"displayOn");
    mock().expectNCalls(8,"show").ignoreOtherParameters();
    mock().expectNCalls(4,"hide").ignoreOtherParameters();

    // Production code
    Display_Init(&testDisplay, &testExternVars);
    Display_On();
    Display_ToggleMode();
    Display_ToggleMode();
    Display_ToggleMode();

    mock().expectNCalls(1,"show").withParameter("region_id", ROW_1);
    mock().expectNCalls(3,"show").withParameter("region_id", ROW_2);
    mock().expectNCalls(1,"show").withParameter("region_id", ROW_3);
    mock().expectNCalls(2,"hide").withParameter("region_id", ROW_1);
    mock().expectNCalls(2,"hide").withParameter("region_id", ROW_3);
    mock().expectNCalls(1,"displayOff");
    mock().expectNCalls(1,"displayOn");

    Display_SetAlarm();
    Display_ShowTime();
    Display_Off();
    Display_On();

    // Checks
    mock().checkExpectations();
}

TEST(DisplayModule, U33_DiurnalConversionValidInput)
{
    // Production code
    uint8_t digit1, digit2, digit3, digit4, digit5;
    msToDiurn(74120000, &digit1, &digit2, &digit3, &digit4, &digit5);

    // Checks
    CHECK_EQUAL(10, digit1);
    CHECK_EQUAL(3, digit2);
    CHECK_EQUAL(6, digit3);
    CHECK_EQUAL(4, digit4);
    CHECK_EQUAL(10, digit5);
}

TEST(DisplayModule, U34_DiurnalConversionInvalidInput)
{
    // Production code
    uint8_t digit1, digit2, digit3, digit4, digit5;
    msToDiurn(97200000, &digit1, &digit2, &digit3, &digit4, &digit5);

    // Checks
    CHECK(digit1 < 12);
    CHECK(digit2 < 12);
    CHECK(digit3 < 12);
    CHECK(digit4 < 12);
    CHECK(digit5 < 12);
}

TEST(DisplayModule, U35_SemiDiurnalConversionValidInput)
{
    // Production code
    uint8_t digit1, digit2, digit3, digit4, digit5;
    msToSemiDiurn(74120000, &digit1, &digit2, &digit3, &digit4, &digit5);

    // Checks
    CHECK_EQUAL(1, digit1);
    CHECK_EQUAL(8, digit2);
    CHECK_EQUAL(7, digit3);
    CHECK_EQUAL(0, digit4);
    CHECK_EQUAL(10, digit5);
}

TEST(DisplayModule, U36_SemiDiurnalConversionInvalidInput)
{
    // Production code
    uint8_t digit1, digit2, digit3, digit4, digit5;
    msToSemiDiurn(97100000, &digit1, &digit2, &digit3, &digit4, &digit5);

    // Checks
    CHECK(digit1 < 2);
    CHECK(digit2 < 12);
    CHECK(digit3 < 12);
    CHECK(digit4 < 12);
    CHECK(digit5 < 12);
}

TEST(DisplayModule, U37_TradConversionValidInput)
{
    // Production code
    uint8_t hr_24, min, sec;
    msToTrad(74120000, &hr_24, &min, &sec);

    // Checks
    CHECK_EQUAL(20, hr_24);
    CHECK_EQUAL(35, min);
    CHECK_EQUAL(20, sec);
}

TEST(DisplayModule, U38_TradConversionInvalidInput)
{
    // Production code
    uint8_t hr_24, min, sec;
    msToTrad(97200000, &hr_24, &min, &sec);

    // Checks
    CHECK(hr_24 < 24);
    CHECK(min < 60);
    CHECK(sec < 60);
}
