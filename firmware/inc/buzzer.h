#ifndef FIRMWARE_INC_BUZZER_H_
#define FIRMWARE_INC_BUZZER_H_

#include "clock_types.h"

#define MAX_VOLUME  100
#define MIN_VOLUME  10

typedef struct buzzer_t
{
    bool is_active;
    uint8_t volume;

    void (*start)(void);
    void (*stop)(void);
    void (*setOutputLevel)(uint32_t dc);
}Buzzer;

ClockStatus Buzzer_Init(Buzzer *self);
ClockStatus Buzzer_Start();
ClockStatus Buzzer_Stop();
ClockStatus Buzzer_SetVolume(uint8_t vol);
ClockStatus Buzzer_SetVolumeMax();
ClockStatus Buzzer_SetVolumeMin();

#endif  // FIRMWARE_INC_BUZZER_H_
