/*
 * time.h
 *
 *  Created on: Nov. 18, 2023
 *      Author: lemck
 */

#ifndef TIME_TRACK_H_
#define TIME_TRACK_H_

#include <stdint.h>

#include "clock_types.h"
#include "main.h"
#include "rtc.h"
#include "gps.h"

typedef struct time_track_t
{
	Rtc *rtc;
	Gps *gps;
}TimeTrack;

ClockStatus TimeTrack_Init(TimeTrack *self);
ClockStatus TimeTrack_Update(TimeTrack *self);
ClockStatus TimeTrack_PeriodicCallback(TimeTrack *self, uint32_t period_ms);


#endif /* TIME_TRACK_H_ */
