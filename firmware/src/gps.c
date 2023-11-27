#include "gps.h"

// Static variables
static uint32_t curr_time_ms;
static uint32_t prev_time_ms;
static uint8_t gps_connected_flag;

// Static function prototypes
static uint32_t gpsTimeToMs(GpsTime time);

ClockStatus Gps_Init(Gps *self)
{
    self->timezone_offset = -5;

    curr_time_ms = 0;
    prev_time_ms = 0;
    gps_connected_flag = 0;

    return CLOCK_OK;
}
uint8_t Gps_Connected(Gps *self)
{
    gps_connected_flag = (uint8_t) self->gpsConnected();

    curr_time_ms = gpsTimeToMs(Gps_GetTime(self));

    // Check if connected flag is not set OR gps time has not changed
    if (!gps_connected_flag || (curr_time_ms == prev_time_ms)) {
        return 0;
    }
    prev_time_ms = curr_time_ms;

    return 1;
}
GpsTime Gps_GetTime(Gps *self)
{
    // Calculate GPS time from driver UTC time
    float utc_time = self->getUtcTime();
    GpsTime t;
    t.hr = (uint8_t) utc_time / 10000;
    t.min = (uint8_t) ((uint8_t) utc_time % 10000) / 100;
    t.sec = (uint8_t) utc_time % 100;

    // Apply timezone offset
    t.hr = t.hr + self->timezone_offset;

    return t;
}

ClockStatus Gps_SetTimezone(Gps *self, int8_t timezone)
{
    self->timezone_offset = timezone;
    return CLOCK_OK;
}

static uint32_t gpsTimeToMs(GpsTime time)
{
    return (uint32_t) time.sec * 1000 + (uint32_t) time.min * 60000
            + (uint32_t) time.hr * 3600000;
}
