#include "buzzer.h"

ClockStatus Buzzer_Init(Buzzer *self)
{
    self->is_active = false;
    self->volume = MIN_VOLUME;

    if (self->startPwm == NULL
        || self->stopPwm == NULL
        || self->setDutyCycle == NULL) {
        return CLOCK_FAIL;
    }

    return CLOCK_OK;
}

ClockStatus Buzzer_Start(Buzzer *self)
{
    if (!self->is_active) {
        self->startPwm();
        self->is_active = true;
    }
    return CLOCK_OK;
}

ClockStatus Buzzer_Stop(Buzzer *self)
{
    if (self->is_active) {
        self->stopPwm();
        self->is_active = false;
    }
    return CLOCK_OK;
}

ClockStatus Buzzer_SetVolume(Buzzer *self, uint8_t vol)
{
    if (vol < MIN_VOLUME || vol > MAX_VOLUME) {
        return CLOCK_FAIL;
    }

    (self->is_active) ? self->stopPwm() : 0;
    self->setDutyCycle(vol*DC_COEFF);
    (self->is_active) ? self->startPwm() : 0;

    return CLOCK_OK;
}

ClockStatus Buzzer_SetVolumeMax(Buzzer *self)
{
    return Buzzer_SetVolume(self, MAX_VOLUME);
}

ClockStatus Buzzer_SetVolumeMin(Buzzer *self)
{
    return Buzzer_SetVolume(self, MIN_VOLUME);
}
