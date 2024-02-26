#ifndef FIRMWARE_INC_CLOCK_TYPES_H_
#define FIRMWARE_INC_CLOCK_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define UNUSED(x) (void) x

// Generic error code return type
typedef enum clockstatus
{
    CLOCK_OK,
    CLOCK_FAIL
} ClockStatus;

// Colour values in 2-state RGB
typedef enum
{
    RED     = 0b100,
    GREEN   = 0b010,
    BLUE    = 0b001,
    YELLOW  = 0b110,
    MAGENTA = 0b101,
    CYAN    = 0b011,
    WHITE   = 0b111,
} Colour;

#endif  // FIRMWARE_INC_CLOCK_TYPES_H_
