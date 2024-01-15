/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include <stdbool.h>
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
#define DISP_TX_Pin GPIO_PIN_2
#define DISP_TX_GPIO_Port GPIOA
#define DISP_RX_Pin GPIO_PIN_3
#define DISP_RX_GPIO_Port GPIOA
#define DISP_RESET_Pin GPIO_PIN_4
#define DISP_RESET_GPIO_Port GPIOA
#define BUZZER_PWM_Pin GPIO_PIN_6
#define BUZZER_PWM_GPIO_Port GPIOA
#define RTC_SQW_Pin GPIO_PIN_9
#define RTC_SQW_GPIO_Port GPIOC
#define RTC_SQW_EXTI_IRQn EXTI4_15_IRQn
#define BTN5_IN_Pin GPIO_PIN_10
#define BTN5_IN_GPIO_Port GPIOC
#define BTN5_IN_EXTI_IRQn EXTI4_15_IRQn
#define BTN6_IN_Pin GPIO_PIN_11
#define BTN6_IN_GPIO_Port GPIOC
#define BTN6_IN_EXTI_IRQn EXTI4_15_IRQn
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
