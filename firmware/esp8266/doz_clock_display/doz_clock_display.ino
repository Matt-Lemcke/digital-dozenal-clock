#include "rgb_matrix.h"
#include "test_bitmap.h"
#include "chrono_uart.h"


Chrono chrono_ctx;

void displayOn(void)
{
  RgbMatrix_RegionWritePixels(&top_region);
  RgbMatrix_RegionWritePixels(&mid_region);
  RgbMatrix_RegionWritePixels(&bot_region);
}

void displayOff(void)
{
  RgbMatrix_Clear();
}

void setBitmap(uint8_t id, uint8_t *vals)
{
  switch(id)
  {
    case TOP_REGION_ID:
      memcpy(top_region.pixel_buffer, vals, top_region.pixel_buffer_size);
      RgbMatrix_RegionWritePixels(&top_region);
      break;
    case MID_REGION_ID:
      memcpy(mid_region.pixel_buffer, vals, mid_region.pixel_buffer_size);
      RgbMatrix_RegionWritePixels(&mid_region);
      break;
    case BOT_REGION_ID:
      memcpy(bot_region.pixel_buffer, vals, bot_region.pixel_buffer_size);
      RgbMatrix_RegionWritePixels(&bot_region);
      break;
  }
}

void setColour(uint8_t id, uint8_t val)
{
  PixelColour colour;
  switch(val)
  {
    case RED_ID:
      colour = Red;
      break;
    case BLUE_ID:
      colour = Blue;
      break;
    case GREEN_ID:
      colour = Green;
      break;
    case WHITE_ID:
      colour = White;
      break;
    case YELLOW_ID:
      colour = Yellow;
      break;
    case CYAN_ID:
      colour = Cyan;
      break;
    case MAGENTA_ID:
      colour = Magenta;
      break;
    default:
      colour = Red;
      break;
  }
  switch(id)
  {
    case TOP_REGION_ID:
      RgbMatrix_RegionSetColour(&top_region, colour);
      break;
    case MID_REGION_ID:
      RgbMatrix_RegionSetColour(&mid_region, colour);
      break;
    case BOT_REGION_ID:
      RgbMatrix_RegionSetColour(&bot_region, colour);
      break;
  }
}

void setMatrixBrightness(uint8_t val)
{
  RgbMatrix_SetBrightness(val);
}

void setRegionStatus(uint8_t id, uint8_t val)
{
  PixelRegion *region;
  bool all_regions = 0;
  switch(id)
  {
    case TOP_REGION_ID:
      region = &top_region;
      break;
    case MID_REGION_ID:
      region = &mid_region;
      break;
    case BOT_REGION_ID:
      region = &bot_region;
      break;
    case ALL_REGION_ID:
      all_regions = 1;
      break;
  }
  switch(val)
  {
    case DISPLAY_OFF_ID:
      if(all_regions)
      {
        RgbMatrix_RegionOff(&top_region);
        RgbMatrix_RegionOff(&mid_region);
        RgbMatrix_RegionOff(&bot_region);
      }
      else 
      {
        RgbMatrix_RegionOff(region);
      }
      break;
    case DISPLAY_ON_ID:
      if(all_regions)
      {
        RgbMatrix_RegionOn(&top_region);
        RgbMatrix_RegionOn(&mid_region);
        RgbMatrix_RegionOn(&bot_region);
      }
      else 
      {
        RgbMatrix_RegionOn(region);
      }
      break;
  }
}

void setup() {
  chrono_ctx.display_off = displayOff;
  chrono_ctx.display_on = displayOn;
  chrono_ctx.setColour = setColour;
  chrono_ctx.setMatrixBrightness = setMatrixBrightness;
  chrono_ctx.setBitmap = setBitmap;
  chrono_ctx.setRegionStatus = setRegionStatus;
  chrono_ctx.setRegionStatus = setRegionStatus;
  Chrono_Init(&chrono_ctx, 9600);
  
  RgbMatrix_Init();
  memcpy(top_region.pixel_buffer, bars_map, top_region.pixel_buffer_size);
  memcpy(mid_region.pixel_buffer, bars_map, mid_region.pixel_buffer_size);
  memcpy(mid_region.pixel_buffer, bars_map, mid_region.pixel_buffer_size);
  memcpy(bot_region.pixel_buffer, bars_map, bot_region.pixel_buffer_size);

  displayOff();
}

void loop() {
  // put your main code here, to run repeatedly:
  Chrono_Update();
}
