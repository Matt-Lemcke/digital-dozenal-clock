#ifndef CHRONO_UART_H
#define CHRONO_UART_H

#include <Arduino.h>
#include "chrono_protocol.h"

typedef struct
{
  void (*display_off)(void);
  void (*display_on)(void);
  void (*setColour)(uint8_t id, uint8_t val);
  void (*setMatrixBrightness)(uint8_t val);
  void (*setBitmap)(uint8_t id, uint8_t *vals);
  void (*setRegionStatus)(uint8_t id, uint8_t val);
}Chrono;


void Chrono_Init(Chrono *ctx, uint32_t br);
void Chrono_Update();

#endif /*CHRONO_UART_H*/
