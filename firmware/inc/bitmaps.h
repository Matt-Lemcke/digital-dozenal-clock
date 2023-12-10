#ifndef FIRMWARE_INC_BITMAPS_H_
#define FIRMWARE_INC_BITMAPS_H_

#define BLANK_CHAR  "  "
#define SOLID_CHAR  "B "

#define SMALL_DIGIT_ROWS  7
#define LARGE_DIGIT_ROWS  12

static char large_numbers[][LARGE_DIGIT_ROWS] = {
    {
        // 0
        0b00111100,
        0b01111110,
        0b11000011,
        0b11000011,
        0b11000011,
        0b11000011,
        0b11000011,
        0b11000011,
        0b11000011,
        0b11000011,
        0b01111110,
        0b00111100
    },
    {
        // 1
    },
    {
        // 2
    },
    {
        // 3
    },
    {
        // 4
    },
    {
        // 5
    },
    {
        // 6
    },
    {
        // 7
    },
    {
        // 8
    },
    {
        // 9
    },
    {
        // 10
    },
    {
        // 11
    },
    {
        // :
    }
};

// digits need to be stored as 8 col wide so leave the first 2 cols as 0s
static char small_numbers[][SMALL_DIGIT_ROWS] = {
    {
        // 0
        0b0001110,
        0b0010001,
        0b0010001,
        0b0010001,
        0b0010001,
        0b0010001,
        0b0001110
    },
    {
        // 1
    },
    {
        // 2
    },
    {
        // 3
    },
    {
        // 4
    },
    {
        // 5
    },
    {
        // 6
    },
    {
        // 7
    },
    {
        // 8
    },
    {
        // 9
    },
    {
        // 10
    },
    {
        // 11
    },
    {
        // :
    },
};

// digits need to be stored as 8 col wide so right align the symbols like with the small numbers
static char small_symbols[][SMALL_DIGIT_ROWS] = {
    {
        // AM
        0b00000000,
        0b00000000,
        0b00100101,
        0b01010111,
        0b01110111,
        0b01010101,
        0b01010101
    },
    {
        // PM
    },
    {
        // alarm
    },
    {
        // timer
    },
    {
        // sun
    },
    {
        // moon
    },
    {
        // !
    },
};

#endif  // FIRMWARE_INC_CLOCK_TYPES_H_
