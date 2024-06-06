/*
 * hub75-driver.c
 *
 *  Created on: Jan 26, 2024
 *      Author: lemck
 */

#include "hub75-driver.h"
#include "tim.h"

#define MIN_PWM_CCR 8000
#define MAX_PWM_CCR 22500
#define BRIGHTNESS_TO_CCR(x)    MAX_PWM_CCR - (MAX_PWM_CCR-MIN_PWM_CCR)/MAX_BRIGHTNESS*x

#define MAX_SCAN_ROWS       16
#define BYTES_PER_ROW       8
#define NUM_COLOUR_LINES    3
#define BUFFER_ROW_SIZE     BYTES_PER_ROW*NUM_COLOUR_LINES
#define BITMAP_SIZE         BUFFER_ROW_SIZE*2*MAX_SCAN_ROWS

#define NUM_REGIONS 3

typedef struct bitmap_region
{
    uint8_t     *p_source;
    uint32_t    size;
    Colour      colour;
    uint8_t     start_row;
    bool        show;
}Bitmap_Region;

/*
    Private function definitions
*/
static inline void set_row_address(uint8_t row_num);
static inline void set_latch(void);
static inline void reset_latch(void);

static void set_oe_pwm_ccr(uint16_t ccr);
static void copy_to_buffer(Bitmap_Region *region);
static void hold_oe_high(void);

/*
    Private variables
*/
static SPI_HandleTypeDef    *hspi;
static TIM_HandleTypeDef    *htim;
static uint32_t             htim_channel;

static uint8_t  row_id      = 0;
static uint16_t  last_ccr   = MIN_PWM_CCR;
static bool     display_on  = false;

/*
 * Bitmap buffer format:
 *
 * Blue Bits Row 16
 * Green Bits Row 16
 * Red Bits Row 16
 * Blue Bits row 0
 * Green Bits Row 0
 * Red Bits Row 0
 * Blue Bits Row 17
 * Green Bits Row 17
 * Red Bits Row 17
 * Blue Bits row 1
 * Green Bits Row 1
 * Red Bits Row 1
 * ...
 */
static uint8_t bitmap_buffer[BITMAP_SIZE];
static Bitmap_Region regions[NUM_REGIONS] =
{
    {
        // Top region
        .start_row = 1,
        .size      = SMALL_REGION_SIZE,
        .colour    = RED,
        .p_source  = NULL,
        .show      = 1,
    },
    {
        // Middle region
        .start_row = 10,
        .size      = LARGE_REGION_SIZE,
        .colour    = BLUE,
        .p_source  = NULL,
        .show      = 1,
    },
    {
        // Bottom region
        .start_row = 24,
        .size      = SMALL_REGION_SIZE,
        .colour    = GREEN,
        .p_source  = NULL,
        .show      = 1,
    },
};

/*
    Public functions
*/
void HUB75_Init(SPI_HandleTypeDef *spi, TIM_HandleTypeDef *tim, uint32_t channel)
{
    hspi = spi;
    htim = tim;
    htim_channel = channel;

    // Clear bitmap buffer
    memset(bitmap_buffer, 0, BITMAP_SIZE);

    // Clear the display memory
    uint8_t zeros[BUFFER_ROW_SIZE];
    memset(zeros, 0, BUFFER_ROW_SIZE);
    reset_latch();
    for(int i = 0; i < 4; i++)
    {
        HAL_SPI_Transmit(hspi, zeros, BUFFER_ROW_SIZE, 500);
    }
    set_latch();
    set_row_address(0);
    reset_latch();

    // Set pmw duty cycle to min
    set_oe_pwm_ccr(MIN_PWM_CCR);

    __HAL_TIM_ENABLE_IT(htim, TIM_IT_UPDATE);
    HAL_TIM_PWM_Start_IT(htim, htim_channel);
}

void HUB75_DisplayOn(void)
{
    // Enable PWM timer
    display_on = true;
    set_oe_pwm_ccr(last_ccr);
}

void HUB75_DisplayOff(void)
{
    // Disable PWM timer
    display_on = false;
    hold_oe_high();
}

void HUB75_SetDisplayBrightness(uint8_t brightness)
{
    if (brightness <= MAX_BRIGHTNESS)
    {
        set_oe_pwm_ccr(BRIGHTNESS_TO_CCR((uint16_t)brightness));
    }
}

void HUB75_SetBitmap(uint8_t region_id, uint8_t *bitmap)
{
    if (region_id < NUM_REGIONS)
    {
        regions[region_id].p_source = bitmap;
        if (regions[region_id].show)
        {
            copy_to_buffer(regions + region_id);
        }
    }
}

void HUB75_SetColour(uint8_t region_id, Colour colour_id)
{
    if (region_id < NUM_REGIONS)
    {
        regions[region_id].colour = colour_id;
        if (regions[region_id].show)
        {
            copy_to_buffer(regions + region_id);
        }
    }
}

void HUB75_Show(uint8_t region_id)
{
    if (region_id < NUM_REGIONS && !regions[region_id].show)
    {
        regions[region_id].show = 1;
        copy_to_buffer(regions + region_id);
    }
}

void HUB75_Hide(uint8_t region_id)
{
    if (region_id < NUM_REGIONS && regions[region_id].show)
    {
        regions[region_id].show = 0;
        copy_to_buffer(regions + region_id);
    }
}

void HUB75_PwmStartPulse(void)
{
    // Triggers when OE is HIGH (display off)
    set_latch();                // Latch data written in last call
    set_row_address(row_id);    // Go to row address of last call
    reset_latch();              // Unlatch

    row_id = (row_id + 1) % MAX_SCAN_ROWS;  // Move to next row

    HAL_SPI_Transmit_DMA(hspi, bitmap_buffer + row_id*BUFFER_ROW_SIZE*2, BUFFER_ROW_SIZE*2);    // Write data for next call
}

/*
    Private functions
*/
inline void set_row_address(uint8_t row_num)
{
    HAL_GPIO_WritePin(DISP_A_GPIO_Port, DISP_A_Pin, row_num & (1<<0));
    HAL_GPIO_WritePin(DISP_B_GPIO_Port, DISP_B_Pin, row_num & (1<<1));
    HAL_GPIO_WritePin(DISP_C_GPIO_Port, DISP_C_Pin, row_num & (1<<2));
    HAL_GPIO_WritePin(DISP_D_GPIO_Port, DISP_D_Pin, row_num & (1<<3));
    HAL_GPIO_WritePin(DISP_E_GPIO_Port, DISP_E_Pin, 0); // Always 1 for 32 row display
}

inline void reset_latch(void)
{
    HAL_GPIO_WritePin(DISP_LAT_GPIO_Port, DISP_LAT_Pin, GPIO_PIN_RESET);
}

inline void set_latch(void)
{
    HAL_GPIO_WritePin(DISP_LAT_GPIO_Port, DISP_LAT_Pin, GPIO_PIN_SET);
}

void set_oe_pwm_ccr(uint16_t ccr)
{
    // Adjust PWM duty cycle
    // Longer OE HIGH time = shorter LED on time = lower brightness
    if (ccr < MIN_PWM_CCR)
    {
        ccr = MIN_PWM_CCR;
    }
    else if (ccr > MAX_PWM_CCR)
    {
        ccr = MAX_PWM_CCR;
    }
    if (display_on)
    {
        htim->Instance->CCR1 = ccr;
    }
    last_ccr = ccr;
}

void hold_oe_high(void)
{
    htim->Instance->CCR1 = htim->Instance->ARR;
}

void copy_to_buffer(Bitmap_Region *region)
{
    uint32_t buffer_row = 0;
    if (region->p_source == NULL)
    {
        return;
    }
    for(int region_row = 0; region_row < region->size / BYTES_PER_ROW; region_row++)
    {
        buffer_row = region_row + region->start_row;
        if (buffer_row < MAX_SCAN_ROWS)
        {
            buffer_row = buffer_row * 2 + 1;
        }
        else
        {
            buffer_row = (buffer_row % MAX_SCAN_ROWS)*2;
        }

        if (!region->show)
        {
            memset(bitmap_buffer + buffer_row*BUFFER_ROW_SIZE, 0, BUFFER_ROW_SIZE);
        }
        else
        {
            for(int i = 0; i < NUM_COLOUR_LINES; i++)
            {
                if (region->colour & (1<<i))
                {
                    memcpy(bitmap_buffer + buffer_row*BUFFER_ROW_SIZE + i*BYTES_PER_ROW, region->p_source + region_row*BYTES_PER_ROW, BYTES_PER_ROW);
                }
                else
                {
                    memset(bitmap_buffer + buffer_row*BUFFER_ROW_SIZE + i*BYTES_PER_ROW, 0, BYTES_PER_ROW);
                }
            }
        }
    }
}
