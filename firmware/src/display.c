#include "display.h"
#include <stdio.h>
#include <string.h>

const char DOZ_NUMERALS[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'X', 'E'};

ClockStatus Display_Init(Display *self)
{
    self->brightness = 100;
    return CLOCK_OK;
}

ClockStatus Display_UpdateTime(Display *self, Time *curr_time, char *output)
{
    UNUSED(self);

    uint32_t time = curr_time->milliseconds;
    char temp[4] = "";
    strcpy(output, "");

    if(curr_time->milliseconds > MILLISECONDS_PER_DAY - 1)
    {
        return CLOCK_FAIL;
    }
    if(curr_time->format == DIURNAL)
    {
        uint8_t doz_digits[5];
        time = time * 2.88 / 1000;
        for(int i=4; i>=0; i--)
        {   
            doz_digits[i] = time % 12;
            printf("%d\t%d\n", doz_digits[i], time);
            time = time / 12;
        }
        for(int i=0; i<=4; i++)
        {
            if(i==3)
            {
                sprintf(temp, ".%c", DOZ_NUMERALS[doz_digits[i]]);
            }
            else
            {
                sprintf(temp, "%c", DOZ_NUMERALS[doz_digits[i]]);
            }
            strcat(output, temp);
        }
    }
    else if(curr_time->format == DEC24H)
    {
        uint8_t dec_digits[3];
        time = time / 1000; // Convert to seconds
        for(int i=2; i>=0; i--)
        {   
            dec_digits[i] = time % 60;
            time = time / 60;
        }
        for(int i=0; i<=2; i++)
        {
            if(dec_digits[i] < 10)
            {
                sprintf(temp, "0%d:", dec_digits[i]);
            }
            else
            {
                sprintf(temp, "%d:", dec_digits[i]);
            }
            strcat(output, temp);
        }
        output[MAX_CHARACTERS] = '\0';
    }
    else
    {
        return CLOCK_FAIL;
    }
    printf("%s\n", output);
    return CLOCK_OK;
}

#ifdef NO_PLATFORM
void main(void) { 
    Time curr_time = {0};
    curr_time.format = DIURNAL;
    curr_time.milliseconds = 900;
    Display clock_screen;
    char output[MAX_CHARACTERS + 1] = "";
    Display_Init(&clock_screen);
    Display_UpdateTime(&clock_screen, &curr_time, output);
    curr_time.format = DEC24H;
    Display_UpdateTime(&clock_screen, &curr_time, output);
}
#endif