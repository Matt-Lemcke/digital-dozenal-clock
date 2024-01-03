/*
 * gpio-display.c
 *
 *  Created on: Jan 1, 2024
 *      Author: lemck
 */


#include "gpio-display.h"
#include <string.h>

#define LED_MATRIX_B2_Pin                       DISP_B2_Pin
#define LED_MATRIX_B2_GPIO_Port                 DISP_B2_GPIO_Port
#define LED_MATRIX_G2_Pin                       DISP_G2_Pin
#define LED_MATRIX_G2_GPIO_Port                 DISP_G2_GPIO_Port
#define LED_MATRIX_B1_Pin                       DISP_B1_Pin
#define LED_MATRIX_B1_GPIO_Port                 DISP_B1_GPIO_Port
#define LED_MATRIX_R1_Pin                       DISP_R1_Pin
#define LED_MATRIX_R1_GPIO_Port                 DISP_R1_GPIO_Port
#define LED_MATRIX_R2_Pin                       DISP_R2_Pin
#define LED_MATRIX_R2_GPIO_Port                 DISP_R2_GPIO_Port
#define LED_MATRIX_G1_Pin                       DISP_G1_Pin
#define LED_MATRIX_G1_GPIO_Port                 DISP_G1_GPIO_Port
#define LED_MATRIX_LATCH_Pin                    DISP_LAT_Pin
#define LED_MATRIX_LATCH_GPIO_Port              DISP_LAT_GPIO_Port
#define LED_MATRIX_ADDRESS_LINE_C_Pin           DISP_C_Pin
#define LED_MATRIX_ADDRESS_LINE_C_GPIO_Port     DISP_C_GPIO_Port
#define LED_MATRIX_CLK_Pin                      DISP_CLK_Pin
#define LED_MATRIX_CLK_GPIO_Port                DISP_CLK_GPIO_Port
#define LED_MATRIX_ADDRESS_LINE_A_Pin           DISP_A_Pin
#define LED_MATRIX_ADDRESS_LINE_A_GPIO_Port     DISP_A_GPIO_Port
#define LED_MATRIX_ADDRESS_LINE_B_Pin           DISP_B_Pin
#define LED_MATRIX_ADDRESS_LINE_B_GPIO_Port     DISP_B_GPIO_Port
#define LED_MATRIX_ADDRESS_LINE_E_Pin           DISP_C_Pin
#define LED_MATRIX_ADDRESS_LINE_E_GPIO_Port     DISP_C_GPIO_Port
#define LED_MATRIX_ADDRESS_LINE_D_Pin           DISP_D_Pin
#define LED_MATRIX_ADDRESS_LINE_D_GPIO_Port     DISP_D_GPIO_Port
#define LED_MATRIX_OE_Pin                       DISP_OE_Pin
#define LED_MATRIX_OE_GPIO_Port                 DISP_OE_GPIO_Port

static void HUB75E_DelayUs(int us);
static void HUB75E_setPin(HUB75EPin p, uint8_t state);

unsigned char graphicsBuffer[TOTAL_BYTES] = {0};
HUB75EDisplayBrightnessLevel brightness;
HUB75EDisplayColor displayColor;
HUB75EAddressingMode addressingMode;

void HUB75E_Init (void)
{

    // Setting clock low
    HUB75E_setPin(PinCLK, 0);

    //Setting all RGB lines low
    HUB75E_setPin(PinRed1, 0);
    HUB75E_setPin(PinRed2, 0);
    HUB75E_setPin(PinGreen1, 0);
    HUB75E_setPin(PinGreen2, 0);
    HUB75E_setPin(PinBlue1, 0);
    HUB75E_setPin(PinBlue2, 0);

    //Setting Address lines to 0x00;
    HUB75E_setPin(PinADDRA, 0);
    HUB75E_setPin(PinADDRB, 0);
    HUB75E_setPin(PinADDRC, 0);
    HUB75E_setPin(PinADDRD, 0);
    HUB75E_setPin(PinADDRE, 0);

}

void HUB75E_clearDisplayBuffer()
{
    memset(graphicsBuffer, 0, TOTAL_BYTES);
}

void HUB75E_setDisplayBrightness(HUB75EDisplayBrightnessLevel b)
{
    brightness = b;
}

void HUB75E_setDisplayColor(HUB75EDisplayColor c)
{
    displayColor = c;
}

void HUB75E_setDisplayBuffer(unsigned char pixels[])
{
//    HUB75E_DisplayOFF();
    memcpy(graphicsBuffer, pixels, TOTAL_BYTES);
}

void HUB75E_DisplayOFF()
{
    HUB75E_setPin(PinOE, 1); //Setting OE high for turning off all LEDs
}

void HUB75E_setAddressingMode(HUB75EAddressingMode addrMode)
{
    addressingMode = addrMode;
}

void HUB75E_displayBufferPixels()
{


    uint8_t upper_pixel_byte = 0;
    uint8_t lower_pixel_byte = 0;
    uint8_t upper_pixel = 0;
    uint8_t lower_pixel = 0;

    for(int row = 0; row < PIXEL_ROWS/2;)
    {
        for(int byte = 0; byte < HORIZONTAL_BYTES; byte++)
        {
            upper_pixel_byte = graphicsBuffer[row*HORIZONTAL_BYTES + byte];
            lower_pixel_byte = graphicsBuffer[(row+PIXEL_ROWS/2)*HORIZONTAL_BYTES + byte];

            for(int i = 7; i >=0;)
            {
                upper_pixel = (upper_pixel_byte >> i) & 1;
                lower_pixel = (lower_pixel_byte >> i) & 1;

                switch(displayColor)
                {//Setting the right display color
                    case Red:
                        HUB75E_setPin(PinRed1, upper_pixel);
                        HUB75E_setPin(PinRed2, lower_pixel);
                        break;
                    case Green:
                        HUB75E_setPin(PinGreen1, upper_pixel);
                        HUB75E_setPin(PinGreen2, lower_pixel);
                        break;
                    case Blue:
                        HUB75E_setPin(PinBlue1, upper_pixel);
                        HUB75E_setPin(PinBlue2, lower_pixel);
                        break;
                    case Yellow:
                        HUB75E_setPin(PinRed1, upper_pixel);
                        HUB75E_setPin(PinRed2, lower_pixel);
                        HUB75E_setPin(PinGreen1, upper_pixel);
                        HUB75E_setPin(PinGreen2, lower_pixel);
                        break;
                    case Pink:
                        HUB75E_setPin(PinRed1, upper_pixel);
                        HUB75E_setPin(PinRed2, lower_pixel);
                        HUB75E_setPin(PinBlue1, upper_pixel);
                        HUB75E_setPin(PinBlue2, lower_pixel);
                        break;
                    case Cyan:
                        HUB75E_setPin(PinGreen1, upper_pixel);
                        HUB75E_setPin(PinGreen2, lower_pixel);
                        HUB75E_setPin(PinBlue1, upper_pixel);
                        HUB75E_setPin(PinBlue2, lower_pixel);
                        break;
                    case White:
                        HUB75E_setPin(PinRed1, upper_pixel);
                        HUB75E_setPin(PinRed2, lower_pixel);
                        HUB75E_setPin(PinGreen1, upper_pixel);
                        HUB75E_setPin(PinGreen2, lower_pixel);
                        HUB75E_setPin(PinBlue1, upper_pixel);
                        HUB75E_setPin(PinBlue2, lower_pixel);
                        break;
                    case Black:
                        //HUB75E_setPin(PinBlue1, (graphicsBuffer[upper_pixel_pointer] >> i) & 1);
                        //HUB75E_setPin(PinBlue2, (graphicsBuffer[lower_pixel_pointer] >> i) & 1);
                        break;
                    default:
                        HUB75E_setPin(PinRed1, upper_pixel);
                        HUB75E_setPin(PinRed2, lower_pixel);
                        break;
                }

                HUB75E_setPin(PinCLK, 1);
                i--; //Clock delay
                HUB75E_setPin(PinCLK, 0);
            }

        }
        HUB75E_setPin(PinADDRA, (row >> 0) & 1);
        HUB75E_setPin(PinADDRB, (row >> 1) & 1);
        HUB75E_setPin(PinADDRC, 0);
        HUB75E_setPin(PinADDRD, (row >> 3) & 1);
        HUB75E_setPin(PinADDRE, (row >> 2) & 1);

        HUB75E_setPin(PinLATCH, 1);
        row++; //Clock delay
        HUB75E_setPin(PinLATCH, 0);

//        HUB75E_setPin(PinOE, 1); //Setting OE high for turning off all LEDs
        HUB75E_setPin(PinOE, 0); //Setting OE high for turning on all LEDs
        upper_pixel_byte = 0;
    }

}

void HUB75E_displayBufferPixelsOld() {

    int upper_pixel_pointer = 0;
    int lower_pixel_pointer = PIXELS_COUNT_IN_BYTES/2;

    for(int row = 0; row < PIXEL_ROWS/2;) {
        for(int column = 0; column < HORIZONTAL_PIXELS_IN_BYTES; column++) { //Shifting pixels for each row. HORIZONTAL_PIXELS_IN_BYTES x 8

            int pB1 = graphicsBuffer[upper_pixel_pointer]; //upper half 8 pixels
            int pB2 = graphicsBuffer[lower_pixel_pointer]; //lower half 8 pixels

            for(int i = 7; i >= 0;) { //Shifting 8 pixels horizontally. MSb First

                int p1 = (pB1 >> i) & 1;
                int p2 = (pB2 >> i) & 1;

                switch(displayColor) {//Setting the right display color
                    case Red:
                        HUB75E_setPin(PinRed1, p1);
                        HUB75E_setPin(PinRed2, p2);
                        break;
                    case Green:
                        HUB75E_setPin(PinGreen1, p1);
                        HUB75E_setPin(PinGreen2, p2);
                        break;
                    case Blue:
                        HUB75E_setPin(PinBlue1, p1);
                        HUB75E_setPin(PinBlue2, p2);
                        break;
                    case Yellow:
                        HUB75E_setPin(PinRed1, p1);
                        HUB75E_setPin(PinRed2, p2);
                        HUB75E_setPin(PinGreen1, p1);
                        HUB75E_setPin(PinGreen2, p2);
                        break;
                    case Pink:
                        HUB75E_setPin(PinRed1, p1);
                        HUB75E_setPin(PinRed2, p2);
                        HUB75E_setPin(PinBlue1, p1);
                        HUB75E_setPin(PinBlue2, p2);
                        break;
                    case Cyan:
                        HUB75E_setPin(PinGreen1, p1);
                        HUB75E_setPin(PinGreen2, p2);
                        HUB75E_setPin(PinBlue1, p1);
                        HUB75E_setPin(PinBlue2, p2);
                        break;
                    case White:
                        HUB75E_setPin(PinRed1, p1);
                        HUB75E_setPin(PinRed2, p2);
                        HUB75E_setPin(PinGreen1, p1);
                        HUB75E_setPin(PinGreen2, p2);
                        HUB75E_setPin(PinBlue1, p1);
                        HUB75E_setPin(PinBlue2, p2);
                        break;
                    case Black:
                        //HUB75E_setPin(PinBlue1, (graphicsBuffer[upper_pixel_pointer] >> i) & 1);
                        //HUB75E_setPin(PinBlue2, (graphicsBuffer[lower_pixel_pointer] >> i) & 1);
                        break;
                    default:
                        HUB75E_setPin(PinRed1, p1);
                        HUB75E_setPin(PinRed2, p2);
                        break;
                }

                HUB75E_setPin(PinCLK, 1);
                i--; //Clock delay
                HUB75E_setPin(PinCLK, 0);
            }

            upper_pixel_pointer++;
            lower_pixel_pointer++;
        }

        switch (addressingMode)
        {
            case HUB75EAddressingModeABCDE:
                //Setting the row address (0 to 31)
                HUB75E_setPin(PinADDRA, (row & ( 1 << 0 )) >> 0);
                HUB75E_setPin(PinADDRB, (row & ( 1 << 1 )) >> 1);
                HUB75E_setPin(PinADDRC, (row & ( 1 << 2 )) >> 2);
                HUB75E_setPin(PinADDRD, (row & ( 1 << 3 )) >> 3);
                HUB75E_setPin(PinADDRE, (row & ( 1 << 4 )) >> 4);
                break;
            case HUB75EAddressingModeAC:
                if(row == 0) {
                    HUB75E_setPin(PinADDRC, 1); //1 for Zeroth row 0 otherwise
                }
                else
                {
                    HUB75E_setPin(PinADDRC, 0);
                }

                //Clocking
                HUB75E_setPin(PinADDRA, 1);
                HUB75E_DelayUs(1);
                HUB75E_setPin(PinADDRA, 0);
                break;
            default:
                break;
        }

        //Latch falling edge for data transfer to output drivers
        HUB75E_setPin(PinLATCH, 1);
        row++; //Clock delay
        HUB75E_setPin(PinLATCH, 0);

        HUB75E_setPin(PinOE, 0); //Setting OE high for turning on all LEDs
        switch(brightness) { //More the delay, more the brightness
            case BrightnessLevel1:
                HUB75E_DelayUs(1);
                break;
            case BrightnessLevel2:
                HUB75E_DelayUs(10);
                break;
            case BrightnessLevel3:
                HUB75E_DelayUs(50);
                break;
            case BrightnessLevel4:
                HUB75E_DelayUs(150);
                break;
            case BrightnessLevel5:
                HUB75E_DelayUs(250);
                break;
            default:
                HUB75E_DelayUs(50);
                break;
        }
        HUB75E_setPin(PinOE, 1); //Setting OE high for turning off all LEDs
    }
}

void HUB75E_DelayUs(int us)
{
    uint32_t startTick = SysTick -> VAL;
    uint32_t delayTicks = us * (SystemCoreClock/1000000);
    while (SysTick -> VAL - startTick < delayTicks);
}

void HUB75E_setPin(HUB75EPin p, uint8_t state)
{

     switch (p)
     {
         case PinRed1:
            HAL_GPIO_WritePin(LED_MATRIX_R1_GPIO_Port, LED_MATRIX_R1_Pin, state);
            break;
         case PinRed2:
            HAL_GPIO_WritePin(LED_MATRIX_R2_GPIO_Port, LED_MATRIX_R2_Pin, state);
            break;
         case PinGreen1:
            HAL_GPIO_WritePin(LED_MATRIX_G1_GPIO_Port, LED_MATRIX_G1_Pin, state);
            break;
         case PinGreen2:
            HAL_GPIO_WritePin(LED_MATRIX_G2_GPIO_Port, LED_MATRIX_G2_Pin, state);
            break;
         case PinBlue1:
            HAL_GPIO_WritePin(LED_MATRIX_B1_GPIO_Port, LED_MATRIX_B1_Pin, state);
            break;
         case PinBlue2:
            HAL_GPIO_WritePin(LED_MATRIX_B2_GPIO_Port, LED_MATRIX_B2_Pin, state);
            break;
         case PinADDRA:
            HAL_GPIO_WritePin(LED_MATRIX_ADDRESS_LINE_A_GPIO_Port, LED_MATRIX_ADDRESS_LINE_A_Pin, state);
            break;
         case PinADDRB:
            HAL_GPIO_WritePin(LED_MATRIX_ADDRESS_LINE_B_GPIO_Port, LED_MATRIX_ADDRESS_LINE_B_Pin, state);
            break;
         case PinADDRC:
            HAL_GPIO_WritePin(LED_MATRIX_ADDRESS_LINE_C_GPIO_Port, LED_MATRIX_ADDRESS_LINE_C_Pin, state);
            break;
         case PinADDRD:
            HAL_GPIO_WritePin(LED_MATRIX_ADDRESS_LINE_D_GPIO_Port, LED_MATRIX_ADDRESS_LINE_D_Pin, state);
            break;
         case PinADDRE:
            HAL_GPIO_WritePin(LED_MATRIX_ADDRESS_LINE_E_GPIO_Port, LED_MATRIX_ADDRESS_LINE_E_Pin, state);
            break;
         case PinCLK:
            HAL_GPIO_WritePin(LED_MATRIX_CLK_GPIO_Port, LED_MATRIX_CLK_Pin, state);
            break;
         case PinLATCH:
            HAL_GPIO_WritePin(LED_MATRIX_LATCH_GPIO_Port, LED_MATRIX_LATCH_Pin, state);
            break;
         case PinOE:
            HAL_GPIO_WritePin(LED_MATRIX_OE_GPIO_Port, LED_MATRIX_OE_Pin, state);
            break;
         default:
            break;
     }

}
