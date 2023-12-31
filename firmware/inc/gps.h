
#ifndef FIRMWARE_INC_GPS_H_
#define FIRMWARE_INC_GPS_H_

#include <stdint.h>

#include "clock_types.h"

typedef struct gps_time_t {
    uint8_t hr;
    uint8_t min;
    uint8_t sec;
} GpsTime;

typedef struct gps_t {
// function pointers to driver functions
} Gps;

ClockStatus Gps_Init(Gps *self);
ClockStatus Gps_Connected(Gps *self);
GpsTime Gps_GetTime(Gps *self);

#endif  // FIRMWARE_INC_GPS_H_
