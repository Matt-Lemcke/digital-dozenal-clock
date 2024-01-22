#include "gps.h"

Gps *g_gps;

ClockStatus Gps_Init(Gps *self)
{
    g_gps = self;
    return CLOCK_OK;
}
ClockStatus Gps_Connected()
{
    return CLOCK_OK;
}
GpsTime Gps_GetTime()
{
    GpsTime zero = { 0 };
    return zero;
}
