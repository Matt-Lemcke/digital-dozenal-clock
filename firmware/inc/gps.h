/*
 * gps.h
 *
 *  Created on: Nov. 18, 2023
 *      Author: lemck
 */

#ifndef GPS_H_
#define GPS_H_

#include "clock_types.h"
#include "main.h"

typedef struct gps_time_t
{
	uint8_t hr;
	uint8_t min;
	uint8_t sec;
} GpsTime;

typedef struct gps_t
{
	// function pointers to driver functions
}Gps;

ClockStatus Gps_Connected(Gps *self);
GpsTime Gps_GetTime(Gps *self);

#endif /* GPS_H_ */
