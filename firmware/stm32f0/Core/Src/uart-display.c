/*
 * uart-display.c
 *
 *  Created on: Jan 7, 2024
 *      Author: Matt
 */

#include "uart-display.h"

#define UART_TIMEOUT    100

static uint8_t start_code       = START_CODE;
static uint8_t on_code          = ON_CODE;
static uint8_t off_code         = OFF_CODE;
static uint8_t brightness_code  = BRIGHTNESS_CODE;
static uint8_t colour_code      = COLOUR_CODE;
static uint8_t status_code      = STATUS_CODE;
static uint8_t bitmap_code      = BITMAP_CODE;
static uint8_t end_code         = END_CODE;
static uint8_t status_on        = DISPLAY_ON_ID;
static uint8_t status_off       = DISPLAY_OFF_ID;

UART_HandleTypeDef *uart;
uint8_t rx_buff[5];
static bool device_ready = 0, device_waiting = 0;
/*
 * Private function definitions
 */
static void reset_driver(void);
static void region_to_chronoId(uint8_t *val);

/*
 * Public functions
 */
bool Esp8266Driver_Init(UART_HandleTypeDef *huart, uint32_t timeout_ms)
{
    uart = huart;
    HAL_UART_Receive_IT(uart, rx_buff, sizeof(rx_buff));
    reset_driver();
    uint32_t start = HAL_GetTick();
    while(!device_ready)
    {
        if(HAL_GetTick() - start > timeout_ms)
        {
            return 0;
        }
    }
    return 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(!device_ready)
    {
        for(int i = 0; i<sizeof(rx_buff); i++)
        {
            if(rx_buff[i] == END_CODE)
            {
                device_waiting = 1;

                // Ready sequence
                HAL_UART_Transmit(uart, &start_code, 1, UART_TIMEOUT);  // Start
                HAL_UART_Transmit(uart, &start_code, 1, UART_TIMEOUT);  // Start
                HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);    // End transmission
                break;
            }
            else if(rx_buff[i] == START_CODE && device_waiting)
            {
                device_ready = 1;
                break;
            }
        }
        HAL_UART_Receive_IT(uart, rx_buff, sizeof(rx_buff));
    }


}

void Esp8266Driver_DisplayOff(void)
{
    // Off sequence
    HAL_UART_Transmit(uart, &start_code, 1, UART_TIMEOUT);  // Start
    HAL_UART_Transmit(uart, &off_code, 1, UART_TIMEOUT);    // Off
    HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);    // End transmission

}

void Esp8266Driver_DisplayOn(void)
{
    // On sequence
    HAL_UART_Transmit(uart, &start_code, 1, UART_TIMEOUT);  // Start
    HAL_UART_Transmit(uart, &on_code, 1, UART_TIMEOUT);     // On
    HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);    // End transmission
}

void Esp8266Driver_SetDisplayBrightness(uint8_t brightness)
{
    if(brightness <= MAX_BRIGHTNESS)
    {
        // Set brightness sequence
        HAL_UART_Transmit(uart, &start_code, 1, UART_TIMEOUT);          // Start
        HAL_UART_Transmit(uart, &brightness_code, 1, UART_TIMEOUT);     // Brightness
        HAL_UART_Transmit(uart, &brightness, 1, UART_TIMEOUT);          // Send brightness value
        HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);            // End transmission
    }
}

void Esp8266Driver_SetBitmap(uint8_t region_id, uint8_t *bitmap)
{
    region_to_chronoId(&region_id);
    uint8_t size = 0;
    if(region_id == MID_REGION_ID)
    {
        size = LARGE_BITMAP_SIZE;
    }
    else
    {
        size = SMALL_BITMAP_SIZE;
    }

    // Set bitmap sequence
    HAL_UART_Transmit(uart, &start_code, 1, UART_TIMEOUT);      // Start
    HAL_UART_Transmit(uart, &bitmap_code, 1, UART_TIMEOUT);     // Bitmap
    HAL_UART_Transmit(uart, &region_id, 1, UART_TIMEOUT);       // Send region id value
    for(int i = 0; i < size; i++)
    {
        HAL_UART_Transmit(uart, bitmap+i, 1, UART_TIMEOUT);     // Send bitmap values
    }

    HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);        // End transmission
}
void Esp8266Driver_SetColour(uint8_t region_id, uint8_t colour_id)
{
    region_to_chronoId(&region_id);
    HAL_UART_Transmit(uart, &start_code, 1, UART_TIMEOUT);      // Start
    HAL_UART_Transmit(uart, &colour_code, 1, UART_TIMEOUT);     // Colour
    HAL_UART_Transmit(uart, &region_id, 1, UART_TIMEOUT);       // Send region id value
    HAL_UART_Transmit(uart, &colour_id, 1, UART_TIMEOUT);       // Send colour id value
    HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);        // End transmission
}
void Esp8266Driver_Show(uint8_t region_id)
{
    region_to_chronoId(&region_id);
    HAL_UART_Transmit(uart, &start_code, 1, UART_TIMEOUT);      // Start
    HAL_UART_Transmit(uart, &status_code, 1, UART_TIMEOUT);     // Status
    HAL_UART_Transmit(uart, &region_id, 1, UART_TIMEOUT);       // Send region id value
    HAL_UART_Transmit(uart, &status_on, 1, UART_TIMEOUT);       // Send ON status
    HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);        // End transmission
}
void Esp8266Driver_Hide(uint8_t region_id)
{
    region_to_chronoId(&region_id);
    HAL_UART_Transmit(uart, &start_code, 1, UART_TIMEOUT);      // Start
    HAL_UART_Transmit(uart, &status_code, 1, UART_TIMEOUT);     // Status
    HAL_UART_Transmit(uart, &region_id, 1, UART_TIMEOUT);       // Send region id value
    HAL_UART_Transmit(uart, &status_off, 1, UART_TIMEOUT);      // Send OFF status
    HAL_UART_Transmit(uart, &end_code, 1, UART_TIMEOUT);        // End transmission
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

void region_to_chronoId(uint8_t *region_num)
{
    switch(*region_num)
    {
    case 1:
        *region_num = TOP_REGION_ID;
        break;
    case 2:
        *region_num = MID_REGION_ID;
        break;
    case 3:
        *region_num = BOT_REGION_ID;
        break;
    default:
        break;
    }
}
