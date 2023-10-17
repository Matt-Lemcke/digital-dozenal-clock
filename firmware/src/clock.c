#include "clock.h"

#include <stdio.h>

#include "hal.h"

int x = 0;

int timer10Callback(void) {
  if (x < 5000 && hal_ok()) {
    return 1;
  }
  return 0;
}

void runClock(void) {
  x++;
  x = x % 10000;
}

#ifdef NO_PLATFORM
int main(void) {
  runClock();
  return 0;
}
#endif
