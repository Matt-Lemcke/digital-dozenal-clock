/*
 * time.c
 *
 *  Created on: Nov. 18, 2023
 *      Author: lemck
 */


#include "time_track.h"

uint8_t check_rtc = 0;
uint8_t gps_lost = 0;
uint16_t n = 0;

uint32_t time_ms = 0;
RtcTime rtc_time = {0}, prev_rtc_time = {0};
GpsTime gps_time = {0};

static uint8_t rtcTimesEqual(RtcTime *time_a, RtcTime *time_b);
static uint32_t rtcTimeToMs(RtcTime *time);
static uint32_t gpsTimeToMs(GpsTime *time);
static void msToRtcTime(uint32_t time_ms, RtcTime *time);

ClockStatus TimeTrack_Init(TimeTrack *self)
{
	time_ms = 0;
	n = 0;
	rtc_time = Rtc_GetTime(self->rtc);
	prev_rtc_time = rtc_time;
	return CLOCK_OK;
}

ClockStatus TimeTrack_Update(TimeTrack *self)
{
	if(check_rtc)
	{
		check_rtc = 0;
		rtc_time = Rtc_GetTime(self->rtc);
		if(!rtcTimesEqual(&rtc_time, &prev_rtc_time))
		{
			// Re-sync internal time to RTC when it updates
			time_ms = rtcTimeToMs(&rtc_time);
			prev_rtc_time = rtc_time;
			n++;
		}

	}
	if(n >= 3600)
	{
		// Attempt to re-sync internal time and RTC to GPS every hour
		if(Gps_Connected(self->gps))
		{
			// GPS connected
			gps_lost = 0;
			gps_time = Gps_GetTime(self->gps);

			time_ms = gpsTimeToMs(&gps_time);	// Sync internal time to GPS

			msToRtcTime(time_ms, &rtc_time);
			if(Rtc_SetTime(self->rtc, &rtc_time) != CLOCK_OK)	// Sync RTC to GPS
			{
				return CLOCK_FAIL;
			}
			prev_rtc_time = rtc_time;
			n = 0;
		}
		else
		{
			// GPS disconnected
			gps_lost = 1;
		}
	}
	return CLOCK_OK;
}

ClockStatus TimeTrack_PeriodicCallback(TimeTrack *self, uint32_t period_ms)
{
	time_ms += period_ms;
	check_rtc = 1;
	return CLOCK_OK;
}

uint8_t rtcTimesEqual(RtcTime *time_a, RtcTime *time_b)
{
	if(time_a->sec != time_b->sec)
	{
		return 0;
	}
	else if(time_a->min != time_b->min)
	{
		return 0;
	}
	else if(time_a->hr != time_b->hr)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

uint32_t rtcTimeToMs(RtcTime *time)
{
	return (uint32_t)time->sec*1000 + (uint32_t)time->min*60000 + (uint32_t)time->hr*3600000;
}

uint32_t gpsTimeToMs(GpsTime *time)
{
	return (uint32_t)time->sec*1000 + (uint32_t)time->min*60000 + (uint32_t)time->hr*3600000;
}

void msToRtcTime(uint32_t time_ms, RtcTime *time)
{
	time_ms = time_ms / 1000;
	time->sec = time_ms % 60;
	time_ms = time_ms / 60;
	time->min = time_ms % 60;
	time_ms = time_ms / 60;
	time->hr = time_ms % 24;
}
