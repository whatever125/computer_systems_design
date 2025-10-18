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
static uint8_t counter_value = 0;  /* Текущее значение счетчика (0-3) */
static uint8_t overflow_count = 0; /* Кол-во переполнений с перезагрузки */
static button_press_type_t current_press = NO_PRESS; /* Тип текущего нажатия */
static uint32_t last_press_start_tick = 0;           /* Тик начала нажатия */
static bool animation_active = false;     /* Флаг активной анимации */
static uint32_t animation_start_tick = 0; /* Тик начала анимации */
static uint8_t animation_step =
    0; /* Шаг анимации (0: мигание обоими, 1: мигание зеленым) */
static uint8_t green_blinks_remaining = 0; /* Оставшиеся мигания зеленым */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void update_counter_display(void);
static void trigger_overflow_animation(void);
static void trigger_underflow_animation(void);
static void update_animation(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
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
  led_driver_init();
  button_driver_init();
  update_counter_display();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    button_update_state();
    current_press = button_get_press_type();
    if (current_press == SHORT_PRESS) {
      /* Короткое нажатие: +1 */
      counter_value = (counter_value + 1U) % 4U;
      if (counter_value == 0U) {
        overflow_count++;
        trigger_overflow_animation();
      }
      update_counter_display();
      last_press_start_tick = HAL_GetTick(); /* Сброс для следующего */
    } else if (current_press == LONG_PRESS) {
      /* Долгое нажатие: -1 */
      if (counter_value == 0U) {
        counter_value = 3U;
        if (overflow_count > 0U) {
          overflow_count--;
          trigger_underflow_animation();
        }
      } else {
        counter_value--;
      }
      update_counter_display();
      last_press_start_tick = HAL_GetTick(); /* Сброс для следующего */
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
void SystemClock_Config(void)
{
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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
 * @brief Обновить отображение счетчика на LED
 */
static void update_counter_display(void) {
  led_set_state(GREEN_LED, (counter_value & 0x01U) ? LED_ON : LED_OFF);
  led_set_state(YELLOW_LED, (counter_value & 0x02U) ? LED_ON : LED_OFF);
}

/**
 * @brief Запустить анимацию переполнения
 */
static void trigger_overflow_animation(void) {
  animation_active = true;
  animation_start_tick = HAL_GetTick();
  animation_step = 0U;
  green_blinks_remaining = overflow_count;
  /* Выключить LED на время анимации */
  led_set_state(GREEN_LED, LED_OFF);
  led_set_state(YELLOW_LED, LED_OFF);
}

/**
 * @brief Запустить анимацию недополнения (аналогично переполнению)
 */
static void trigger_underflow_animation(void) {
  /* Та же логика, что и для overflow */
  trigger_overflow_animation();
}

/**
 * @brief Обновить состояние анимации
 */
static void update_animation(void) {
  uint32_t current_tick = HAL_GetTick();
  uint32_t elapsed = current_tick - animation_start_tick;

  if (animation_step == 0U) {
    /* Шаг 0: Задержка перед анимацией */
    if (elapsed >= ANIMATION_CYCLE_MS) {
      /* Закончили задержку */
      animation_step = 1U;
      animation_start_tick = current_tick;
      elapsed = 0U;
    } else {
      /* Выключить LED на время анимации */
      led_set_state(GREEN_LED, LED_OFF);
      led_set_state(YELLOW_LED, LED_OFF);
      return;
    }
  }

  if (animation_step == 1U) {
    /* Шаг 1: Мигание обоими LED (5 циклов по 200 мс) */
    if (elapsed >= (OVERFLOW_ANIMATION_CYCLES * 2U * ANIMATION_CYCLE_MS)) {
      /* Закончили мигание обоими */
      animation_step = 2U;
      animation_start_tick = current_tick;
      elapsed = 0U;
    } else {
      /* Мигание: ON 200мс, OFF 200мс */
      bool led_on = (elapsed % (2U * ANIMATION_CYCLE_MS)) < ANIMATION_CYCLE_MS;
      led_set_state(GREEN_LED, led_on ? LED_ON : LED_OFF);
      led_set_state(YELLOW_LED, led_on ? LED_ON : LED_OFF);
      return;
    }
  }

  if (animation_step == 2U) {
    /* Шаг 2: Мигание зеленым (green_blinks_remaining раз по 300 мс) */
    if (green_blinks_remaining == 0U) {
      /* Анимация завершена */
      animation_active = false;
      update_counter_display(); /* Вернуться к счетчику */
      return;
    }

    if (elapsed >= (2U * GREEN_BLINK_CYCLE_MS)) {
      /* Один цикл мигания завершен */
      green_blinks_remaining--;
      animation_start_tick = current_tick;
      elapsed = 0U;
    } else {
      /* Мигание: ON 300мс, OFF 300мс */
      bool led_on =
          (elapsed % (2U * GREEN_BLINK_CYCLE_MS)) < GREEN_BLINK_CYCLE_MS;
      led_set_state(GREEN_LED, led_on ? LED_ON : LED_OFF);
      led_set_state(YELLOW_LED, LED_OFF); /* Желтый выключен */
      return;
    }
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
     file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
