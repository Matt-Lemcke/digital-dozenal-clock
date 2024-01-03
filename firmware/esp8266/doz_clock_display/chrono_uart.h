#ifndef CHRONO_PROTOCOL_H
#define CHRONO_PROTOCOL_H

#include <Arduino.h>

#define START_CODE  'S' // Need to change to 0xAA, this is for testing only
#define END_CODE    'E' // Need to change to 0x44, this is for testing only

#define BITMAP_CODE     'a'
#define COLOUR_CODE     'b'
#define BRIGHTNESS_CODE 'c'
#define OFF_CODE        'd'
#define ON_CODE         'e'

#define TOP_REGION_ID   '1'
#define MID_REGION_ID   '2'
#define BOT_REGION_ID   '3'

#define RED_ID      '!'
#define BLUE_ID     '@'
#define GREEN_ID    '#'
#define WHITE_ID    '$'
#define CYAN_ID     '%'
#define YELLOW_ID   '^'
#define MAGENTA_ID  '&'

typedef struct
{
  void (*display_off)(void);
  void (*display_on)(void);
  void (*setColour)(uint8_t id, uint8_t val);
  void (*setMatrixBrightness)(uint8_t val);
  void (*setBitmap)(uint8_t id, uint8_t *vals);
}Chrono;


void Chrono_Init(Chrono *ctx, uint32_t br);
void Chrono_Update();

#endif /*CHRONO_PROTOCOL_H*/
