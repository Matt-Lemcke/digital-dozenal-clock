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
void getTime(uint8_t *hour_24mode, uint8_t *minute, uint8_t *second)
{
    mock().actualCall("getTime").withOutputParameter("hour_24mode", hour_24mode).withOutputParameter("minute", minute).withOutputParameter("second", second);
}

/*
    Test Groups
*/

TEST_GROUP(TimeTrackAlgorithm)
{
    void setup()
    {
        testRtc.getTime = getTime;
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

TEST(TimeTrackAlgorithm, U11_SyncToRtc)
{

    uint8_t *hr = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *min = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *sec = (uint8_t *)malloc(sizeof(uint8_t));

    // Setup mock function calls
    *hr = 8;
    *min = 0;
    *sec = 0;

    mock().expectNCalls(2,"getTime")
        .withOutputParameterReturning("hour_24mode", hr, sizeof(hr))
        .withOutputParameterReturning("minute", min, sizeof(min))
        .withOutputParameterReturning("second", sec, sizeof(sec));

    // Production code
    uint32_t Output_ms;
    TimeTrack_Init();
    TimeTrack_SyncToRtc();
    TimeTrack_GetTimeMs(&Output_ms);

    free(hr);
    free(min);
    free(sec);

    // Checks
    CHECK_EQUAL(28800000, Output_ms);
}

TEST(TimeTrackAlgorithm, U12_RtcIncrement)
{

    uint8_t *hr = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *min = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *sec = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *sec2 = (uint8_t *)malloc(sizeof(uint8_t));

    // Setup mock function calls
    *hr = 8;
    *min = 0;
    *sec = 0;
    mock().expectNCalls(2,"getTime")
        .withOutputParameterReturning("hour_24mode", hr, sizeof(hr))
        .withOutputParameterReturning("minute", min, sizeof(min))
        .withOutputParameterReturning("second", sec, sizeof(sec));

    *sec2 = 1;
    mock().expectNCalls(1,"getTime")
        .withOutputParameterReturning("hour_24mode", hr, sizeof(hr))
        .withOutputParameterReturning("minute", min, sizeof(min))
        .withOutputParameterReturning("second", sec2, sizeof(sec2));
    
    // Production code
    uint32_t Output_ms1, Output_ms2; 
    TimeTrack_Init();
    TimeTrack_PeriodicCallback(0);
    TimeTrack_Update();
    TimeTrack_GetTimeMs(&Output_ms1);
    TimeTrack_PeriodicCallback(0);
    TimeTrack_Update();
    TimeTrack_GetTimeMs(&Output_ms2);

    free(hr);
    free(min);
    free(sec);
    free(sec2);

    // Checks
    CHECK_EQUAL(28800000, Output_ms1);
    CHECK_EQUAL(28801000, Output_ms2);
}

TEST(TimeTrackAlgorithm, U13_RtcRollover)
{

    uint8_t *hr = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *min = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *sec = (uint8_t *)malloc(sizeof(uint8_t));

    uint8_t *hr2 = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *min2 = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *sec2 = (uint8_t *)malloc(sizeof(uint8_t));

    // Setup mock function calls
    *hr = 23;
    *min = 59;
    *sec = 59;
    mock().expectNCalls(2,"getTime")
        .withOutputParameterReturning("hour_24mode", hr, sizeof(hr))
        .withOutputParameterReturning("minute", min, sizeof(min))
        .withOutputParameterReturning("second", sec, sizeof(sec));

    *hr2 = 0;
    *min2 = 0;
    *sec2 = 0;
    mock().expectNCalls(1,"getTime")
        .withOutputParameterReturning("hour_24mode", hr2, sizeof(hr2))
        .withOutputParameterReturning("minute", min2, sizeof(min2))
        .withOutputParameterReturning("second", sec2, sizeof(sec2));
    
    // Production code
    uint32_t Output_ms1, Output_ms2; 
    TimeTrack_Init();
    TimeTrack_PeriodicCallback(0);
    TimeTrack_Update();
    TimeTrack_GetTimeMs(&Output_ms1);
    TimeTrack_PeriodicCallback(0);
    TimeTrack_Update();
    TimeTrack_GetTimeMs(&Output_ms2);

    free(hr);
    free(min);
    free(sec);
    free(hr2);
    free(min2);
    free(sec2);

    // Checks
    CHECK_EQUAL(86399000, Output_ms1);
    CHECK_EQUAL(0, Output_ms2);
}

TEST(TimeTrackAlgorithm, U15_UpdatesWithRtcConstant)
{

    uint8_t *hr = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *min = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *sec = (uint8_t *)malloc(sizeof(uint8_t));

    // Setup mock function calls
    *hr = 8;
    *min = 0;
    *sec = 0;
    mock().expectNCalls(2,"getTime")
        .withOutputParameterReturning("hour_24mode", hr, sizeof(hr))
        .withOutputParameterReturning("minute", min, sizeof(min))
        .withOutputParameterReturning("second", sec, sizeof(sec));
    
    // Production code
    uint32_t Output_ms1, Output_ms2; 
    TimeTrack_Init();
    TimeTrack_GetTimeMs(&Output_ms1);
    TimeTrack_PeriodicCallback(100);
    TimeTrack_GetTimeMs(&Output_ms2);

    free(hr);
    free(min);
    free(sec);

    // Checks
    CHECK_EQUAL(28800000, Output_ms1);
    CHECK_EQUAL(28800100, Output_ms2);
}

TEST(TimeTrackAlgorithm, U16_UpdatesWithRtcIncrements)
{

    uint8_t *hr = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *min = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *sec = (uint8_t *)malloc(sizeof(uint8_t));

    uint8_t *sec2 = (uint8_t *)malloc(sizeof(uint8_t));

    // Setup mock function calls
    *hr = 8;
    *min = 0;
    *sec = 0;
    mock().expectNCalls(1,"getTime")
        .withOutputParameterReturning("hour_24mode", hr, sizeof(hr))
        .withOutputParameterReturning("minute", min, sizeof(min))
        .withOutputParameterReturning("second", sec, sizeof(sec));

    *sec2 = 1;
    mock().expectNCalls(1,"getTime")
        .withOutputParameterReturning("hour_24mode", hr, sizeof(hr))
        .withOutputParameterReturning("minute", min, sizeof(min))
        .withOutputParameterReturning("second", sec2, sizeof(sec2));
    
    // Production code
    uint32_t Output_ms1, Output_ms2; 
    TimeTrack_Init();
    TimeTrack_GetTimeMs(&Output_ms1);
    TimeTrack_PeriodicCallback(0);
    TimeTrack_Update();
    TimeTrack_PeriodicCallback(100);
    TimeTrack_GetTimeMs(&Output_ms2);

    free(hr);
    free(min);
    free(sec);
    free(sec2);

    // Checks
    CHECK_EQUAL(28800000, Output_ms1);
    CHECK_EQUAL(28801100, Output_ms2);
}

TEST(TimeTrackAlgorithm, U17_UpdatesRollover)
{

    uint8_t *hr = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *min = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *sec = (uint8_t *)malloc(sizeof(uint8_t));

    uint8_t *hr2 = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *min2 = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *sec2 = (uint8_t *)malloc(sizeof(uint8_t));

    uint8_t *hr3 = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *min3 = (uint8_t *)malloc(sizeof(uint8_t));
    uint8_t *sec3 = (uint8_t *)malloc(sizeof(uint8_t));

    // Setup mock function calls
    *hr = 23;
    *min = 59;
    *sec = 59;
    mock().expectNCalls(1,"getTime")
        .withOutputParameterReturning("hour_24mode", hr, sizeof(hr))
        .withOutputParameterReturning("minute", min, sizeof(min))
        .withOutputParameterReturning("second", sec, sizeof(sec));

    *hr2 = 0;
    *min2 = 0;
    *sec2 = 0;
    mock().expectNCalls(1,"getTime")
        .withOutputParameterReturning("hour_24mode", hr2, sizeof(hr2))
        .withOutputParameterReturning("minute", min2, sizeof(min2))
        .withOutputParameterReturning("second", sec2, sizeof(sec2));

    *hr3 = 0;
    *min3 = 0;
    *sec3 = 1;
    mock().expectNCalls(1,"getTime")
        .withOutputParameterReturning("hour_24mode", hr3, sizeof(hr3))
        .withOutputParameterReturning("minute", min3, sizeof(min3))
        .withOutputParameterReturning("second", sec3, sizeof(sec3));
    
    // Production code
    uint32_t Output_ms1, Output_ms2, Output_ms3; 
    TimeTrack_Init();
    TimeTrack_PeriodicCallback(999);
    TimeTrack_GetTimeMs(&Output_ms1);
    TimeTrack_Update();
    TimeTrack_PeriodicCallback(999);
    TimeTrack_GetTimeMs(&Output_ms2);
    TimeTrack_Update();
    TimeTrack_PeriodicCallback(999);
    TimeTrack_GetTimeMs(&Output_ms3);

    free(hr);
    free(min);
    free(sec);
    free(hr2);
    free(min2);
    free(sec2);
    free(hr3);
    free(min3);
    free(sec3);

    // Checks
    CHECK_EQUAL(86399999, Output_ms1);
    CHECK_EQUAL(999, Output_ms2);
    CHECK_EQUAL(1999, Output_ms3);
}
