#include "gps.h"

// Static variables
static uint32_t curr_time_ms;
static uint32_t prev_time_ms;
static uint8_t gps_connected_flag;
static float utc_time;
static GpsTime gps_time = { 0 };

// Static function prototypes
static uint32_t gpsTimeToMs(GpsTime time);

ClockStatus Gps_Init(Gps *self)
{
    self->timezone_offset = -5;

    curr_time_ms = 0;
    prev_time_ms = 0;
    gps_connected_flag = 0;
    utc_time = 0;

    return CLOCK_OK;
}
uint8_t Gps_Connected(Gps *self)
{
    // Check if connected flag is not set
    gps_connected_flag = (uint8_t) self->gpsConnected();
    if (!gps_connected_flag) {
        return 0;
    }

    // Check if gps time has not changed
    curr_time_ms = gpsTimeToMs(Gps_GetTime(self));
    if (curr_time_ms == prev_time_ms) {
        return 0;
    }
    prev_time_ms = curr_time_ms;

    return 1;
}
GpsTime Gps_GetTime(Gps *self)
{
    // Get driver UTC time
    utc_time = self->getUtcTime();

    // Calculate gps hour, min and sec from UTC time and user inputted timezone
    int8_t hr = ((uint8_t) utc_time / 10000) + self->timezone_offset;
    if (hr < 0) {
        gps_time.hr = (uint8_t) 24 + hr;
    } else if (hr > 23) {
        gps_time.hr = (uint8_t) hr % 24;
    } else {
        gps_time.hr = (uint8_t) hr;
    }
    gps_time.min = (uint8_t) ((uint8_t) utc_time % 10000) / 100;
    gps_time.sec = (uint8_t) utc_time % 100;

    return gps_time;
}

ClockStatus Gps_SetTimezone(Gps *self, int8_t timezone)
{
    if (timezone < -12 || timezone > 14) {
        return CLOCK_FAIL;
    }
    self->timezone_offset = timezone;
    return CLOCK_OK;
}

static uint32_t gpsTimeToMs(GpsTime time)
{
    return (uint32_t) time.sec * 1000 + (uint32_t) time.min * 60000
            + (uint32_t) time.hr * 3600000;
}
