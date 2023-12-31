/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "user.h"
#include "sim7600.h"

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
#define GSM_PW_EN_Pin GPIO_PIN_2
#define GSM_PW_EN_GPIO_Port GPIOC
#define GSM_POWERKEY_Pin GPIO_PIN_3
#define GSM_POWERKEY_GPIO_Port GPIOC
#define GSM_RI_Pin GPIO_PIN_2
#define GSM_RI_GPIO_Port GPIOA
#define GSM_DTR_Pin GPIO_PIN_3
#define GSM_DTR_GPIO_Port GPIOA
#define CAN_STBY_Pin GPIO_PIN_10
#define CAN_STBY_GPIO_Port GPIOA
#define USER_BTN_Pin GPIO_PIN_3
#define USER_BTN_GPIO_Port GPIOB
#define USER_BTN_EXTI_IRQn EXTI3_IRQn
#define USER_LED2_Pin GPIO_PIN_0
#define USER_LED2_GPIO_Port GPIOE
#define USER_LED1_Pin GPIO_PIN_1
#define USER_LED1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
