#include "display.h"
#include <stdio.h>
#include <string.h>

void Display_Init(Display *self)
{
    self->brightness = 100;
}

void Display_UpdateTime(Display *self, Time *curr_time)
{
    char output[MAX_CHARACTERS + 1] = "";
    uint32_t time = curr_time->milliseconds;
    if(curr_time->format == DIURNAL)
    {

    }
    else if(curr_time->format == DEC24H)
    {
        char temp[4] = "";
        time = time / 1000; // Convert to seconds
        for(int i=0; i<2; i++)
        {   
            if(time % 60 < 10)
            {
                sprintf(temp, ":0%s", time % 60);
            }
            else
            {
                sprintf(temp, ":%s", time % 60);
            }
            // put temp into output
            time = time / 60;
        }
    }
    printf("Output: %s\n", output);
}

void main(void) { 
    Time curr_time = {0};
    curr_time.format = DEC24H;
    curr_time.milliseconds = 1000000;
    Display clock_screen;
    Display_Init(&clock_screen);
    Display_UpdateTime(&clock_screen, &curr_time);
}