/*
 * adc-light-sens.c
 *
 *  Created on: Jan 13, 2024
 *      Author: Matt L.
 */


#include "adc-light-sens.h"
#include "event_queue.h"

#define ADC_BUF_LENGTH  10
#define MAX_LIGHT_LEVEL 4095    // Max value on 12-bit ADC
#define ALPHA           30      // Alpha value for moving avg. (out of 100)


#define LIGHT_THRESHOLD         900
#define DARK_THRESHOLD          20
#define LIGHT_TRANSITION_BUFF   100
#define DARK_TRANSITION_BUFF    5

ADC_HandleTypeDef *adc;
TIM_HandleTypeDef *tim;
static uint16_t adc_buffer[ADC_BUF_LENGTH];
static uint32_t sample_avg;
static uint32_t moving_avg;
static bool is_dim_room, is_dark_room;

void LightSens_Init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim)
{
    adc = hadc;
    tim = htim;
    is_dim_room = 0;
    is_dark_room = 0;
    moving_avg = LIGHT_THRESHOLD;
    HAL_TIM_Base_Start_IT(tim);
}

void LightSens_AdcStartConversion(void)
{
    HAL_ADC_Start_DMA(adc, (uint32_t*)adc_buffer, ADC_BUF_LENGTH);
}

void LightSens_AdcConversionCallback(void)
{
    HAL_ADC_Stop_DMA(adc);
    // Basic average calculation for current sample
    sample_avg = 0;
    for(int i = 0; i < ADC_BUF_LENGTH; i++)
    {
        sample_avg += adc_buffer[i];
    }
    sample_avg = sample_avg / ADC_BUF_LENGTH;

    // Moving average filter applied to samples
    moving_avg = (moving_avg * ALPHA + sample_avg * (100-ALPHA)) / 100;

    if((is_dim_room || is_dark_room) && moving_avg > (LIGHT_THRESHOLD + LIGHT_TRANSITION_BUFF))
    {
        is_dim_room = 0;
        is_dark_room = 0;
        EventQ_TriggerLightEvent(LIGHT_ROOM);
    }
    else if(!is_dark_room && moving_avg < (DARK_THRESHOLD - DARK_TRANSITION_BUFF))
    {
        is_dim_room = 0;
        is_dark_room = 1;
        EventQ_TriggerLightEvent(DARK_ROOM);
    }
    else if(!is_dim_room && moving_avg < (LIGHT_THRESHOLD - LIGHT_TRANSITION_BUFF) && moving_avg > (DARK_THRESHOLD + DARK_TRANSITION_BUFF))
    {
        is_dim_room = 1;
        is_dark_room = 0;
        EventQ_TriggerLightEvent(DIM_ROOM);
    }
}
