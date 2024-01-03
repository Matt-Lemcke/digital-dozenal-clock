#include "rgb_matrix.h"
#include <PxMatrix.h>
#include <Ticker.h>

#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

#define TOTAL_BYTES (PIXEL_COLS*PIXEL_ROWS/8)

/*
 * Private variables
 */
uint8_t display_draw_time=10; //30-70 is usually fine
PxMATRIX display(PIXEL_COLS,PIXEL_ROWS,P_LAT, P_OE,P_A,P_B,P_C,P_D);
Ticker display_ticker;

// Some standard colors
uint16_t red = display.color565(255, 0, 0);
uint16_t green = display.color565(0, 255, 0);
uint16_t blue = display.color565(0, 0, 255);
uint16_t white = display.color565(255, 255, 255);
uint16_t yellow = display.color565(255, 255, 0);
uint16_t cyan = display.color565(0, 255, 255);
uint16_t magenta = display.color565(255, 0, 255);
uint16_t black = display.color565(0, 0, 0);

/*
 * Private function declaration
 */
static void display_update_enable(bool is_enable);
static void display_updater(void);


void RgbMatrix_Init(void)
{
  Serial.begin(9600);
  display.begin(16);

  display.clearDisplay();
  display_update_enable(true);
}

void RgbMatrix_RegionWritePixels(PixelRegion *region, uint8_t pixels[])
{
  uint8_t first_col_index = 0;
  uint8_t pixel_byte = 0;
  for(int y = 0; y < region->height; y++)
  {
    first_col_index = y * region->width / 8;
    for(int x_byte = 0; x_byte < region->width/8; x_byte++)
    {
      pixel_byte = pixels[first_col_index + x_byte];
      for(int i = 7; i >= 0; i--)
      {
        if((pixel_byte >> i) & 1)
        {
          display.drawPixel(x_byte*8 + (7 - i), y, red);
        }
        else
        {
          display.drawPixel(x_byte*8 + (7 - i), y, black);
        }
      }
    }
  }
}

/*
 * Private functions
 */

// Disable/enable refresh timer
void display_update_enable(bool is_enable)
{
  if (is_enable)
    display_ticker.attach(0.004, display_updater);
  else
    display_ticker.detach();
}

// ISR for display refresh
void display_updater()
{
  display.display(display_draw_time);
}
