#ifndef CHRONO_PROTOCOL_H
#define CHRONO_PROTOCOL_H

#define START_CODE  0xAA
#define END_CODE    0x55

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

//#define START_CODE  'S'
//#define END_CODE    'E'
//
//#define BITMAP_CODE     '1'
//#define COLOUR_CODE     '2'
//#define BRIGHTNESS_CODE '3'
//#define STATUS_CODE     '4'
//#define ON_CODE         '5'
//#define OFF_CODE        '6'
//
//#define TOP_REGION_ID   't'
//#define MID_REGION_ID   'm'
//#define BOT_REGION_ID   'b'
//#define ALL_REGION_ID   'a'
//
//#define DISPLAY_ON_ID   'n'
//#define DISPLAY_OFF_ID  'f'
//
//#define RED_ID      'R'
//#define BLUE_ID     'B'
//#define GREEN_ID    'G'
//#define WHITE_ID    'W'
//#define CYAN_ID     'C'
//#define YELLOW_ID   'Y'
//#define MAGENTA_ID  'M'

#define MAX_BRIGHTNESS  255

#define LARGE_BITMAP_SIZE 96
#define SMALL_BITMAP_SIZE 56

#endif /*CHRONO_PROTOCOL_H*/
