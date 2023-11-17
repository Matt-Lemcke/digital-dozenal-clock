#include "display.h"

#include <stdio.h>
#include <string.h>

const char DOZ_NUMERALS[] = {'0', '1', '2', '3', '4', '5',
                             '6', '7', '8', '9', 'X', 'E'};

char output[20] = "";
char temp[4] = "";
uint8_t digits[5];

ClockStatus Display_Init(Display *self) {
  self->brightness = 100;
  return CLOCK_OK;
}

ClockStatus Display_UpdateTime(Display *self, uint32_t time_ms,
                               TimeFormat format) {
  UNUSED(self);

  strcpy(output, "");
  strcpy(temp, "");

  if (time_ms > MILLISECONDS_PER_DAY - 1) {
    return CLOCK_FAIL;
  }
  if (format == DIURNAL) {
    time_ms = time_ms * 2.88 / 1000;
    for (int i = 4; i >= 0; i--) {
      digits[i] = time_ms % 12;
      time_ms = time_ms / 12;
    }
    for (int i = 0; i <= 4; i++) {
      if (i == 3) {
        sprintf(temp, ".%c", DOZ_NUMERALS[digits[i]]);
      } else {
        sprintf(temp, "%c", DOZ_NUMERALS[digits[i]]);
      }
      strcat(output, temp);
    }
  } else if (format == DEC24H) {
    time_ms = time_ms / 1000;  // Convert to seconds
    for (int i = 2; i >= 0; i--) {
      digits[i] = time_ms % 60;
      time_ms = time_ms / 60;
    }
    for (int i = 0; i <= 2; i++) {
      if (digits[i] < 10) {
        sprintf(temp, "0%d:", digits[i]);
      } else {
        sprintf(temp, "%d:", digits[i]);
      }
      strcat(output, temp);
    }
    output[MAX_CHARACTERS] = '\0';
  } else {
    return CLOCK_FAIL;
  }

  if (self->clear != NULL && self->sendString != NULL) {
    self->clear();
    self->sendString(output);
  }

  return CLOCK_OK;
}
