#include "buzzer.h"

Buzzer *g_buzzer;

ClockStatus Buzzer_Init(Buzzer *self)
{
    g_buzzer = self;
    g_buzzer->is_active = false;
    g_buzzer->volume = MIN_VOLUME;

    if (g_buzzer->startPwm == NULL
        || g_buzzer->stopPwm == NULL
        || g_buzzer->setDutyCycle == NULL) {
        return CLOCK_FAIL;
    }
    Buzzer_SetVolumeMin(g_buzzer);
    return CLOCK_OK;
}

ClockStatus Buzzer_Start()
{
    if (!g_buzzer->is_active) {
        g_buzzer->startPwm();
        g_buzzer->is_active = true;
    }
    return CLOCK_OK;
}

ClockStatus Buzzer_Stop()
{
    if (g_buzzer->is_active) {
        g_buzzer->stopPwm();
        g_buzzer->is_active = false;
    }
    return CLOCK_OK;
}

ClockStatus Buzzer_SetVolume(uint8_t vol)
{
    if (vol < MIN_VOLUME || vol > MAX_VOLUME) {
        return CLOCK_FAIL;
    }
    g_buzzer->volume = vol;
    if (g_buzzer->is_active) {g_buzzer->stopPwm();}
    g_buzzer->setDutyCycle(g_buzzer->volume*DC_COEFF);
    if (g_buzzer->is_active) {g_buzzer->startPwm();}

    return CLOCK_OK;
}

ClockStatus Buzzer_SetVolumeMax()
{
    return Buzzer_SetVolume(MAX_VOLUME);
}

ClockStatus Buzzer_SetVolumeMin()
{
    return Buzzer_SetVolume(MIN_VOLUME);
}
