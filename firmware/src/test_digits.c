#include "bitmaps.h"
#include "stdio.h"

void printLargeDigit(char array[][LARGE_DIGIT_ROWS], char index)
{
    printf("\n");
    for(int i=0; i<LARGE_DIGIT_ROWS; i++)
    {
        for(int j=7; j>=0; j--)
        {
            if(array[index][i] & 1<<j)
            {
                printf(SOLID_CHAR);
            }
            else
            {
                printf(BLANK_CHAR);
            }
        }
        printf("\n");
    }
}

void printSmallDigit(char array[][SMALL_DIGIT_ROWS], char index)
{
    printf("\n");
    for(int i=0; i<SMALL_DIGIT_ROWS; i++)
    {
        for(int j=7; j>=0; j--)
        {
            if(array[index][i] & 1<<j)
            {
                printf(SOLID_CHAR);
            }
            else
            {
                printf(BLANK_CHAR);
            }
        }
        printf("\n");
    }
}

int main(void)
{
    printLargeDigit(large_numbers, 0);
    printSmallDigit(small_numbers, 0);
    printSmallDigit(small_symbols, 0);
    return 0;
}
