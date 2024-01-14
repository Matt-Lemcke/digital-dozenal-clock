/*
 * uart-display.c
 *
 *  Created on: Jan 7, 2024
 *      Author: Matt
 */

#include "uart-display.h"

#define UART_TIMEOUT    1000


UART_HandleTypeDef *uart;

static uint8_t on_seq[]         = {START_CODE, ON_CODE};
static uint8_t off_seq[]        = {START_CODE, OFF_CODE};
static uint8_t brightness_seq[] = {START_CODE, BRIGHTNESS_CODE};
static uint8_t colour_seq[]     = {START_CODE, COLOUR_CODE};
static uint8_t status_seq[]     = {START_CODE, STATUS_CODE};
static uint8_t bitmap_seq[]     = {START_CODE, BITMAP_CODE};

static uint8_t end_code     = END_CODE;
static uint8_t status_on    = DISPLAY_ON_ID;
static uint8_t status_off   = DISPLAY_OFF_ID;

/*
 * Private function definitions
 */
static void reset_driver(void);

/*
 * Public functions
 */
void Esp8266Driver_Init(UART_HandleTypeDef *huart)
{
    uart = huart;
    reset_driver();
}

void Esp8266Driver_DisplayOff(void)
{
    HAL_UART_Transmit(uart, off_seq, sizeof(off_seq), UART_TIMEOUT);  // Start sequence
    HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);              // End transmission
}

void Esp8266Driver_DisplayOn(void)
{
    HAL_UART_Transmit(uart, on_seq, sizeof(on_seq), UART_TIMEOUT);    // Start sequence
    HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);              // End transmission
}

void Esp8266Driver_SetDisplayBrightness(uint8_t brightness)
{
    if(brightness <= MAX_BRIGHTNESS)
    {
        HAL_UART_Transmit(uart, brightness_seq, sizeof(brightness_seq), UART_TIMEOUT);    // Start sequence
        HAL_UART_Transmit(uart, &brightness, 1, UART_TIMEOUT);                            // Send brightness value
        HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);                              // End transmission
    }
}

void Esp8266Driver_SetBitmap(uint8_t region_id, uint8_t *bitmap)
{
    uint8_t size = 0;
    if(region_id == MID_REGION_ID)
    {
        size = LARGE_BITMAP_SIZE;
    }
    else
    {
        size = SMALL_BITMAP_SIZE;
    }
    HAL_UART_Transmit(uart, bitmap_seq, sizeof(bitmap_seq), UART_TIMEOUT);    // Start sequence
    HAL_UART_Transmit(uart, &region_id, 1, UART_TIMEOUT);                     // Send region id value
    HAL_UART_Transmit(uart, bitmap, size, UART_TIMEOUT);                      // Send bitmap
    HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);                      // End transmission
}
void Esp8266Driver_SetColour(uint8_t region_id, uint8_t colour_id)
{
    HAL_UART_Transmit(uart, colour_seq, sizeof(colour_seq), UART_TIMEOUT);    // Start sequence
    HAL_Delay(1);
    HAL_UART_Transmit(uart, &region_id, 1, UART_TIMEOUT);                     // Send region id value
    HAL_Delay(1);
    HAL_UART_Transmit(uart, &colour_id, 1, UART_TIMEOUT);                     // Send colour id value
    HAL_Delay(1);
    HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);                      // End transmission
    HAL_Delay(1);
}
void Esp8266Driver_Show(uint8_t region_id)
{
    HAL_UART_Transmit(uart, status_seq, sizeof(status_seq), UART_TIMEOUT);    // Start sequence
    HAL_UART_Transmit(uart, &region_id, 1, UART_TIMEOUT);                     // Send region id value
    HAL_UART_Transmit(uart, &status_on, 1, UART_TIMEOUT);                     // Send ON status
    HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);                      // End transmission
}
void Esp8266Driver_Hide(uint8_t region_id)
{
    HAL_UART_Transmit(uart, status_seq, sizeof(status_seq), UART_TIMEOUT);    // Start sequence
    HAL_UART_Transmit(uart, &region_id, 1, UART_TIMEOUT);                     // Send region id value
    HAL_UART_Transmit(uart, &status_off, 1, UART_TIMEOUT);                    // Send OFF status
    HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);                      // End transmission
}

/*
 * Private functions
 */
void reset_driver(void)
{
    // Reset pin pulled High internally, pull low briefly to reset
    HAL_GPIO_WritePin(DISP_RESET_GPIO_Port, DISP_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(DISP_RESET_GPIO_Port, DISP_RESET_Pin, GPIO_PIN_SET);
}
