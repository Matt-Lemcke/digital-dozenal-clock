#ifndef RGB_MATRIX_H
#define RGB_MATRIX_H

#include <Arduino.h>

#define PIXEL_COLS 64
#define PIXEL_ROWS 32

typedef enum {
  Low,
  Med,
  High
}BrightnessLevel;

typedef enum {
  Red,
  Green,
  Blue,
  White,
  Yellow,
  Cyan,
  Magenta,
  Black
}PixelColour;

typedef struct {
  uint8_t height;
  uint8_t width;
  uint8_t topleft_x;
  uint8_t topleft_y;
  PixelColour colour;
  uint8_t is_active;
}PixelRegion;

static PixelRegion region1 = {
  .height = PIXEL_ROWS,
  .width = PIXEL_COLS,
  .topleft_x = 0,
  .topleft_y = 0,
  .colour = Red,
  .is_active = 1
};

void RgbMatrix_Init();
void RgbMatrix_Update();

void RgbMatrix_DisplayOff(void);
void RgbMatrix_DisplayOn(void);
void RgbMatrix_SetBrightness(BrightnessLevel level);

void RgbMatrix_RegionOff(PixelRegion *region);
void RgbMatrix_RegionOn(PixelRegion *region);
void RgbMatrix_RegionSetColour(PixelRegion *region, PixelColour colour);
void RgbMatrix_RegionWritePixels(PixelRegion *region, uint8_t pixels[]);


#endif /* RGB_MATRIX_H */
