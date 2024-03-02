/*
 * rtc-internal.h
 *
 *  Created on: Feb 29, 2024
 *      Author: lemck
 */

#ifndef INC_RTC_INTERNAL_H_
#define INC_RTC_INTERNAL_H_

#include "main.h"

#define ALARM1_ID   1
#define ALARM2_ID   2

void RTC_Init(RTC_HandleTypeDef *rtc);
void RTC_SetTime(uint8_t hr, uint8_t min, uint8_t sec);
void RTC_GetTime(uint8_t *hr, uint8_t *min, uint8_t *sec);
void RTC_SetDay(uint8_t d);
void RTC_GetDay(uint8_t *d);
void RTC_SetMonth(uint8_t m);
void RTC_GetMonth(uint8_t *m);
void RTC_SetAlarm(uint8_t id, uint8_t hr, uint8_t min, uint8_t sec);
void RTC_GetAlarm(uint8_t id, uint8_t *hr, uint8_t *min, uint8_t *sec);
void RTC_EnableAlarm(uint8_t id, bool enable);

#endif /* INC_RTC_INTERNAL_H_ */
