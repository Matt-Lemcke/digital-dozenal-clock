#include "rgb_matrix.h"
#include "test_bitmap.h"

void setup() {


  RgbMatrix_Init();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  RgbMatrix_RegionWritePixels(&region1, bars_map);
  delay(500);
  RgbMatrix_RegionWritePixels(&region1, bars_map_inv);
  delay(500);
}
