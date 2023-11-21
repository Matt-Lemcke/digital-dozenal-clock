#include "gps.h"

ClockStatus Gps_Init(Gps *self)
{
    UNUSED(self);
    return CLOCK_OK;
}
ClockStatus Gps_Connected(Gps *self)
{
    UNUSED(self);
    return CLOCK_OK;
}
GpsTime Gps_GetTime(Gps *self)
{
    UNUSED(self);
    GpsTime zero = { 0 };
    return zero;
}
