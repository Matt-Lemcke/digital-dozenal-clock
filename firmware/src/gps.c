#include "gps.h"

static Gps *g_gps;

ClockStatus Gps_Init(Gps *self)
{
    g_gps = self;
    return CLOCK_OK;
}
ClockStatus Gps_Connected()
{
    if (g_gps->gpsConnected != NULL && g_gps->gpsConnected())
    {
        return CLOCK_OK;
    }
    return CLOCK_FAIL;
}
GpsTime Gps_GetTime()
{
    GpsTime time = { 0 };
    if (g_gps->getUtcTime != NULL)
    {
        int utc_time = (int)g_gps->getUtcTime();
        time.sec = utc_time % 100;
        utc_time = utc_time/100;
        time.min = utc_time % 100;
        utc_time = utc_time/100;
        time.hr  = utc_time % 100;
    }
    return time;
}
