#ifndef FIRMWARE_INC_BUZZER_H_
#define FIRMWARE_INC_BUZZER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "clock_types.h"

#define MAX_VOLUME  100
#define MIN_VOLUME  10
#define DC_COEFF    1     // **Calibrate

typedef struct buzzer_t
{
    bool is_active;
    uint8_t volume;

    void (*startPwm)(void);
    void (*stopPwm)(void);
    void (*setDutyCycle)(uint8_t dc);
}Buzzer;

ClockStatus Buzzer_Init(Buzzer *self);
ClockStatus Buzzer_Start(Buzzer *self);
ClockStatus Buzzer_Stop(Buzzer *self);
ClockStatus Buzzer_SetVolume(Buzzer *self, uint8_t vol);
ClockStatus Buzzer_SetVolumeMax(Buzzer *self);
ClockStatus Buzzer_SetVolumeMin(Buzzer *self);

#endif  // FIRMWARE_INC_BUZZER_H_
