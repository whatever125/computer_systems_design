/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define Button_Pin GPIO_PIN_15
#define Button_GPIO_Port GPIOC
#define Green_LED_Pin GPIO_PIN_13
#define Green_LED_GPIO_Port GPIOD
#define Red_LED_Pin GPIO_PIN_14
#define Red_LED_GPIO_Port GPIOD
#define Yellow_LED_Pin GPIO_PIN_15
#define Yellow_LED_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
#define DEBOUNCE_TIME_MS 50          /* Время дебаунсинга, мс */
#define SHORT_PRESS_THRESHOLD_MS 500 /* Порог короткого нажатия, мс */
#define LONG_PRESS_THRESHOLD_MS 500  /* Порог длинного нажатия, мс */
#define ANIMATION_CYCLE_MS 200       /* Цикл мигания обоими LED, мс */
#define GREEN_BLINK_CYCLE_MS 400     /* Цикл мигания зеленым, мс */
#define OVERFLOW_ANIMATION_CYCLES 5 /* Кол-во циклов мигания обоими */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
