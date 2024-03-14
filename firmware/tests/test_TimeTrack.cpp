extern "C"
{
#include <string.h>

#include "time_track.h"
#include "clock_types.h"
#include "rtc_module.h"
}
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

Rtc testRtc;

/*
    Mock Functions
*/
uint8_t getHour(void)
{
    return mock().actualCall("getHour").returnIntValue();   
}

uint8_t getMinute(void)
{
    return mock().actualCall("getMinute").returnIntValue();   
}

uint8_t getSecond(void)
{
    return mock().actualCall("getSecond").returnIntValue();   
}

/*
    Test Groups
*/

TEST_GROUP(TimeTrackAlgorithm)
{
    void setup()
    {
        // testRtc.getHour = getHour;
        // testRtc.getMinute = getMinute;
        // testRtc.getSecond = getSecond;
        Rtc_Init(&testRtc);
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

// TEST(TimeTrackAlgorithm, U11_SyncToRtc)
// {
//     // Setup mock function calls
//     mock().expectNCalls(2,"getHour").andReturnValue(8);
//     mock().expectNCalls(2,"getMinute").andReturnValue(0);
//     mock().expectNCalls(2,"getSecond").andReturnValue(0);

//     // Production code
//     uint32_t Output_ms;
//     TimeTrack_Init();
//     TimeTrack_SyncToRtc();
//     TimeTrack_GetTimeMs(&Output_ms);

//     // Checks
//     CHECK_EQUAL(28800000, Output_ms);
// }

// TEST(TimeTrackAlgorithm, U12_RtcIncrement)
// {
//     // Setup mock function calls
//     mock().expectNCalls(2,"getHour").andReturnValue(8);
//     mock().expectNCalls(2,"getMinute").andReturnValue(0);
//     mock().expectNCalls(2,"getSecond").andReturnValue(0);

//     mock().expectNCalls(1,"getHour").andReturnValue(8);
//     mock().expectNCalls(1,"getMinute").andReturnValue(0);
//     mock().expectNCalls(1,"getSecond").andReturnValue(1);
    
//     // Production code
//     uint32_t Output_ms1, Output_ms2; 
//     TimeTrack_Init();
//     TimeTrack_PeriodicCallback(0);
//     TimeTrack_Update();
//     TimeTrack_GetTimeMs(&Output_ms1);
//     TimeTrack_PeriodicCallback(0);
//     TimeTrack_Update();
//     TimeTrack_GetTimeMs(&Output_ms2);

//     // Checks
//     CHECK_EQUAL(28800000, Output_ms1);
//     CHECK_EQUAL(28801000, Output_ms2);
// }

// TEST(TimeTrackAlgorithm, U13_RtcRollover)
// {
//     // Setup mock function calls
//     mock().expectNCalls(2,"getHour").andReturnValue(23);
//     mock().expectNCalls(2,"getMinute").andReturnValue(59);
//     mock().expectNCalls(2,"getSecond").andReturnValue(59);

//     mock().expectNCalls(1,"getHour").andReturnValue(0);
//     mock().expectNCalls(1,"getMinute").andReturnValue(0);
//     mock().expectNCalls(1,"getSecond").andReturnValue(0);
    
//     // Production code
//     uint32_t Output_ms1, Output_ms2; 
//     TimeTrack_Init();
//     TimeTrack_PeriodicCallback(0);
//     TimeTrack_Update();
//     TimeTrack_GetTimeMs(&Output_ms1);
//     TimeTrack_PeriodicCallback(0);
//     TimeTrack_Update();
//     TimeTrack_GetTimeMs(&Output_ms2);

//     // Checks
//     CHECK_EQUAL(86399000, Output_ms1);
//     CHECK_EQUAL(0, Output_ms2);
// }

// TEST(TimeTrackAlgorithm, U15_UpdatesWithRtcConstant)
// {
//     // Setup mock function calls
//     mock().expectNCalls(2,"getHour").andReturnValue(8);
//     mock().expectNCalls(2,"getMinute").andReturnValue(0);
//     mock().expectNCalls(2,"getSecond").andReturnValue(0);
    
//     // Production code
//     uint32_t Output_ms1, Output_ms2; 
//     TimeTrack_Init();
//     TimeTrack_GetTimeMs(&Output_ms1);
//     TimeTrack_PeriodicCallback(100);
//     TimeTrack_Update();
//     TimeTrack_GetTimeMs(&Output_ms2);


//     // Checks
//     CHECK_EQUAL(28800000, Output_ms1);
//     CHECK_EQUAL(28800100, Output_ms2);
// }

// TEST(TimeTrackAlgorithm, U16_UpdatesWithRtcIncrements)
// {
//     // Setup mock function calls
//     mock().expectNCalls(1,"getHour").andReturnValue(8);
//     mock().expectNCalls(1,"getMinute").andReturnValue(0);
//     mock().expectNCalls(1,"getSecond").andReturnValue(0);

//     mock().expectNCalls(1,"getHour").andReturnValue(8);
//     mock().expectNCalls(1,"getMinute").andReturnValue(0);
//     mock().expectNCalls(1,"getSecond").andReturnValue(1);
    
//     // Production code
//     uint32_t Output_ms1, Output_ms2; 
//     TimeTrack_Init();
//     TimeTrack_GetTimeMs(&Output_ms1);
//     TimeTrack_PeriodicCallback(0);
//     TimeTrack_Update();
//     TimeTrack_PeriodicCallback(100);
//     TimeTrack_GetTimeMs(&Output_ms2);

//     // Checks
//     CHECK_EQUAL(28800000, Output_ms1);
//     CHECK_EQUAL(28801100, Output_ms2);
// }

// // TEST(TimeTrackAlgorithm, U17_UpdatesRollover)
// // {
// //     // Setup mock function calls
// //     mock().expectNCalls(3,"getHour").andReturnValue(23);
// //     mock().expectNCalls(3,"getMinute").andReturnValue(59);
// //     mock().expectNCalls(3,"getSecond").andReturnValue(59);
    
// //     // Production code
// //     uint32_t Output_ms1, Output_ms2; 
// //     TimeTrack_Init();
// //     TimeTrack_PeriodicCallback(999);
// //     TimeTrack_Update();
// //     TimeTrack_GetTimeMs(&Output_ms1);
// //     TimeTrack_PeriodicCallback(999);
// //     TimeTrack_Update();
// //     TimeTrack_GetTimeMs(&Output_ms2);


// //     // Checks
// //     CHECK_EQUAL(86399999, Output_ms1);
// //     CHECK_EQUAL(1998, Output_ms2);
// // }
