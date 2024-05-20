/*
 * rtc-internal.h
 *
 *  Created on: Feb 29, 2024
 *      Author: lemck
 */

#ifndef INC_RTC_INTERNAL_H_
#define INC_RTC_INTERNAL_H_

#include "main.h"

#define ALARM_ID   1
#define TIMER_ID   2

#define MAX_CALIBRATION_OFFSET  0x1FF

void RTC_Init(RTC_HandleTypeDef *rtc);
void RTC_SetTime(uint8_t hr, uint8_t min, uint8_t sec);
void RTC_GetTime(uint8_t *hr, uint8_t *min, uint8_t *sec);
void RTC_SetDay(uint8_t d);
uint8_t RTC_GetDay(void);
void RTC_SetMonth(uint8_t m);
uint8_t RTC_GetMonth(void);
void RTC_SetAlarm(uint8_t id, uint8_t hr, uint8_t min, uint8_t sec);
void RTC_GetAlarm(uint8_t id, uint8_t *hr, uint8_t *min, uint8_t *sec);
void RTC_EnableAlarm(uint8_t id, bool enable);
bool RTC_SetCalibration(int32_t calib);
int32_t RTC_GetCalibration(void);

bool RTC_CheckDataSaved(RTC_HandleTypeDef *rtc);
bool RTC_CheckAlarmSaved(uint8_t id);

#endif /* INC_RTC_INTERNAL_H_ */
