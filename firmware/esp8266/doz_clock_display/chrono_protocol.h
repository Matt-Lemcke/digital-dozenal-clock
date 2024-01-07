#ifndef CHRONO_PROTOCOL_H
#define CHRONO_PROTOCOL_H

#define START_CODE  0xAA
#define END_CODE    0x44

#define BITMAP_CODE     0xC1
#define COLOUR_CODE     0xC2
#define BRIGHTNESS_CODE 0xC3
#define STATUS_CODE     0xC4
#define ON_CODE         0xC5
#define OFF_CODE        0xC6

#define TOP_REGION_ID   0xE1
#define MID_REGION_ID   0xE2
#define BOT_REGION_ID   0xE3
#define ALL_REGION_ID   0xE4

#define DISPLAY_ON_ID   0xD1
#define DISPLAY_OFF_ID  0xD2

#define RED_ID      0x81
#define BLUE_ID     0x82
#define GREEN_ID    0x83
#define WHITE_ID    0x84
#define CYAN_ID     0x85
#define YELLOW_ID   0x86
#define MAGENTA_ID  0x87

#endif /*CHRONO_PROTOCOL_H*/
