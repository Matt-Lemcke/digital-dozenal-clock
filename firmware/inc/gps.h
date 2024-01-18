
#ifndef FIRMWARE_INC_GPS_H_
#define FIRMWARE_INC_GPS_H_

#include "clock_types.h"

typedef struct gps_time_t {
    uint8_t hr;
    uint8_t min;
    uint8_t sec;
} GpsTime;

typedef struct gps_t {
    int8_t timezone_offset;

    float (*getUtcTime)(void);
    unsigned (*gpsConnected)(void);
} Gps;

ClockStatus Gps_Init(Gps *self);
uint8_t Gps_Connected(Gps *self);
GpsTime Gps_GetTime(Gps *self);
ClockStatus Gps_SetTimezone(Gps *self, int8_t timezone);

#endif  // FIRMWARE_INC_GPS_H_
