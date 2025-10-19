/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "stdint.h"
#include "led_driver.h"
#include "button_driver.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint8_t counter_value = 0;
static uint8_t overflow_count = 0;
static press_type_t current_press = NO_PRESS;
static bool animation_active = false;
static uint32_t animation_start_tick = 0;
static uint8_t animation_step = 0;
static uint8_t green_blinks_remaining = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void update_counter_leds(void);
static void trigger_overflow_animation(void);
static void update_animation(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  update_counter_leds();
  button_driver_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    button_update_state();
    current_press = button_get_press_type();

    if (current_press == SHORT_PRESS) {
      counter_value = (counter_value + 1) % 4;
      if (counter_value == 0) {
        overflow_count++;
        trigger_overflow_animation();
      }
      update_counter_leds();

    } else if (current_press == LONG_PRESS) {
      if (counter_value == 0) {
        counter_value = 3;
        if (overflow_count > 0) {
          overflow_count--;
          trigger_overflow_animation();
        }
      } else {
        counter_value--;
      }
      update_counter_leds();
    }

    if (animation_active) {
      update_animation();
    }
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
   */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
 * Update LEDs to reflect current counter value
 */
static void update_counter_leds(void) {
  led_set_state(GREEN_LED, (counter_value & 0x1) ? LED_ON : LED_OFF);
  led_set_state(YELLOW_LED, (counter_value & 0x2) ? LED_ON : LED_OFF);
}

/**
 * Start overflow animation
 */
static void trigger_overflow_animation(void) {
  // turn off LEDs before animation
  led_set_state(GREEN_LED, LED_OFF);
  led_set_state(YELLOW_LED, LED_OFF);
  // prepare state
  animation_active = true;
  animation_start_tick = HAL_GetTick();
  animation_step = 0;
  green_blinks_remaining = overflow_count;
}

/**
 * Update animation state
 */
static void update_animation(void) {
  uint32_t current_tick = HAL_GetTick();
  uint32_t elapsed_time = current_tick - animation_start_tick;

  if (animation_step == 0) {
    // step 0: initial delay
    if (elapsed_time >= ANIMATION_HALF_CYCLE_MS) {
      // stop initial delay, move to step 1
      animation_step = 1;
      animation_start_tick = current_tick;
      elapsed_time = 0;
    } else {
      // turn off LEDs during initial delay
      led_set_state(GREEN_LED, LED_OFF);
      led_set_state(YELLOW_LED, LED_OFF);
      return;
    }
  }

  if (animation_step == 1) {
    // step 1: blinking both LEDs
    if (elapsed_time >= (OVERFLOW_ANIMATION_CYCLES * ANIMATION_CYCLE_MS)) {
      // move to step 2
      animation_step = 2;
      animation_start_tick = current_tick;
      elapsed_time = 0;
    } else {
      // blink both LEDs
      bool led_on =
          (elapsed_time % ANIMATION_CYCLE_MS) < ANIMATION_HALF_CYCLE_MS;
      led_set_state(GREEN_LED, led_on ? LED_ON : LED_OFF);
      led_set_state(YELLOW_LED, led_on ? LED_ON : LED_OFF);
      return;
    }
  }

  if (animation_step == 2) {
    // step 2: blink green LED
    if (green_blinks_remaining == 0) {
      // animation finished
      animation_active = false;
      update_counter_leds();
      return;
    }

    if (elapsed_time >= GREEN_BLINK_CYCLE_MS) {
      // one green blink cycle finished
      green_blinks_remaining--;
      animation_start_tick = current_tick;
      elapsed_time = 0;
    } else {
      // blink green LED
      bool led_on =
          (elapsed_time % GREEN_BLINK_CYCLE_MS) < GREEN_BLINK_HALF_CYCLE_MS;
      led_set_state(GREEN_LED, led_on ? LED_ON : LED_OFF);
      led_set_state(YELLOW_LED, LED_OFF);
      return;
    }
  }
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state
   */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
     file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
