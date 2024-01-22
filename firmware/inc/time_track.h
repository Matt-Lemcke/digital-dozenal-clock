/*
 * time.h
 *
 *  Created on: Nov. 18, 2023
 *      Author: lemck
 */

#ifndef FIRMWARE_INC_TIME_TRACK_H_
#define FIRMWARE_INC_TIME_TRACK_H_

#include "clock_types.h"

ClockStatus TimeTrack_Init();
ClockStatus TimeTrack_SyncToRtc();
ClockStatus TimeTrack_Update();
ClockStatus TimeTrack_PeriodicCallback(uint32_t period_ms);
ClockStatus TimeTrack_GetTimeMs(uint32_t *output_ms);

#endif /* FIRMWARE_INC_TIME_TRACK_H_ */
