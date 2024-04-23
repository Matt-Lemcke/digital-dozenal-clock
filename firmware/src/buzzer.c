#include "buzzer.h"

static Buzzer *g_buzzer;

ClockStatus Buzzer_Init(Buzzer *self)
{
    g_buzzer = self;
    g_buzzer->is_active = false;
    g_buzzer->volume = MIN_VOLUME;

    if (g_buzzer->start == NULL
        || g_buzzer->stop == NULL
        || g_buzzer->setOutputLevel == NULL) {
        return CLOCK_FAIL;
    }
    Buzzer_SetVolumeMin();
    return CLOCK_OK;
}

ClockStatus Buzzer_Start()
{
    if (!g_buzzer->is_active) {
        g_buzzer->start();
        g_buzzer->is_active = true;
    }
    return CLOCK_OK;
}

ClockStatus Buzzer_Stop()
{
    if (g_buzzer->is_active) {
        g_buzzer->stop();
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
    if (g_buzzer->is_active) {g_buzzer->stop();}
    g_buzzer->setOutputLevel(g_buzzer->volume);
    if (g_buzzer->is_active) {g_buzzer->start();}

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
