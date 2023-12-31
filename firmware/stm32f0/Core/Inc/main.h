/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BTN12_IN_Pin GPIO_PIN_13
#define BTN12_IN_GPIO_Port GPIOC
#define BTN12_IN_EXTI_IRQn EXTI4_15_IRQn
#define I2C_SDA_Pin GPIO_PIN_0
#define I2C_SDA_GPIO_Port GPIOF
#define I2C_SCL_Pin GPIO_PIN_1
#define I2C_SCL_GPIO_Port GPIOF
#define BTN1_IN_Pin GPIO_PIN_0
#define BTN1_IN_GPIO_Port GPIOC
#define BTN1_IN_EXTI_IRQn EXTI0_1_IRQn
#define BTN2_IN_Pin GPIO_PIN_1
#define BTN2_IN_GPIO_Port GPIOC
#define BTN2_IN_EXTI_IRQn EXTI0_1_IRQn
#define BTN3_IN_Pin GPIO_PIN_2
#define BTN3_IN_GPIO_Port GPIOC
#define BTN3_IN_EXTI_IRQn EXTI2_3_IRQn
#define BTN4_IN_Pin GPIO_PIN_3
#define BTN4_IN_GPIO_Port GPIOC
#define BTN4_IN_EXTI_IRQn EXTI2_3_IRQn
#define PHOTO_IN_Pin GPIO_PIN_0
#define PHOTO_IN_GPIO_Port GPIOA
#define BUZZER_PWM_Pin GPIO_PIN_6
#define BUZZER_PWM_GPIO_Port GPIOA
#define J3_PA7_Pin GPIO_PIN_7
#define J3_PA7_GPIO_Port GPIOA
#define DSPL_A_Pin GPIO_PIN_4
#define DSPL_A_GPIO_Port GPIOC
#define DSPL_B_Pin GPIO_PIN_5
#define DSPL_B_GPIO_Port GPIOC
#define DSPL_C_Pin GPIO_PIN_0
#define DSPL_C_GPIO_Port GPIOB
#define DSPL_D_Pin GPIO_PIN_1
#define DSPL_D_GPIO_Port GPIOB
#define DSPL_E_Pin GPIO_PIN_2
#define DSPL_E_GPIO_Port GPIOB
#define DSPL_CLK_Pin GPIO_PIN_10
#define DSPL_CLK_GPIO_Port GPIOB
#define DSPL_LAT_Pin GPIO_PIN_11
#define DSPL_LAT_GPIO_Port GPIOB
#define DSPL_OE_Pin GPIO_PIN_12
#define DSPL_OE_GPIO_Port GPIOB
#define DSPL_B2_Pin GPIO_PIN_13
#define DSPL_B2_GPIO_Port GPIOB
#define DSPL_B1_Pin GPIO_PIN_14
#define DSPL_B1_GPIO_Port GPIOB
#define DSPL_G2_Pin GPIO_PIN_15
#define DSPL_G2_GPIO_Port GPIOB
#define DSPL_G1_Pin GPIO_PIN_6
#define DSPL_G1_GPIO_Port GPIOC
#define DSPL_R2_Pin GPIO_PIN_7
#define DSPL_R2_GPIO_Port GPIOC
#define DSPL_R1_Pin GPIO_PIN_8
#define DSPL_R1_GPIO_Port GPIOC
#define RTC_SQW_Pin GPIO_PIN_9
#define RTC_SQW_GPIO_Port GPIOC
#define RTC_SQW_EXTI_IRQn EXTI4_15_IRQn
#define USART_TX_Pin GPIO_PIN_9
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_10
#define USART_RX_GPIO_Port GPIOA
#define BTN6_IN_Pin GPIO_PIN_10
#define BTN6_IN_GPIO_Port GPIOC
#define BTN6_IN_EXTI_IRQn EXTI4_15_IRQn
#define BTN5_IN_Pin GPIO_PIN_11
#define BTN5_IN_GPIO_Port GPIOC
#define BTN5_IN_EXTI_IRQn EXTI4_15_IRQn
#define BTN7_IN_Pin GPIO_PIN_4
#define BTN7_IN_GPIO_Port GPIOB
#define BTN7_IN_EXTI_IRQn EXTI4_15_IRQn
#define BTN8_IN_Pin GPIO_PIN_5
#define BTN8_IN_GPIO_Port GPIOB
#define BTN8_IN_EXTI_IRQn EXTI4_15_IRQn
#define BTN9_IN_Pin GPIO_PIN_6
#define BTN9_IN_GPIO_Port GPIOB
#define BTN9_IN_EXTI_IRQn EXTI4_15_IRQn
#define BTN10_IN_Pin GPIO_PIN_7
#define BTN10_IN_GPIO_Port GPIOB
#define BTN10_IN_EXTI_IRQn EXTI4_15_IRQn
#define BTN11_IN_Pin GPIO_PIN_8
#define BTN11_IN_GPIO_Port GPIOB
#define BTN11_IN_EXTI_IRQn EXTI4_15_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
