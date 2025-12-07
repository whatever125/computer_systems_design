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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ctype.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "Custom/Drivers/led_driver.h"
#include "Custom/Drivers/button_driver.h"
#include "Custom/Drivers/uart_driver.h"
#include "Custom/Drivers/sound_driver.h"
#include "Custom/Drivers/melody_driver.h"
#include "Custom/Drivers/melody_parser.h"
#include "Custom/Drivers/keyboard_driver.h"
#include "Custom/Drivers/mode_switcher.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
  bool edit_mode;
  char input_buffer[128];
  uint8_t input_index;
} app_state_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static app_state_t app_state = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void process_key(uint8_t key_code);
static void send_status_message(void);
static void enter_edit_mode(void);
static void process_input_buffer(void);
static void play_melody(uint8_t melody_index);
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
  MX_USART6_UART_Init();
  MX_TIM1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  button_driver_init();
  uart_driver_init(UART_POLLING);
  sound_driver_init();
  melody_driver_init();
  keyboard_init();
  mode_switcher_init();

  uart_send_string("\r\nMusical Box with Keyboard Ready\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    mode_switcher_update();
    melody_update();

    if (keyboard_has_event()) {
      uint8_t key = keyboard_get_pressed_key();

      if (get_current_mode() == MODE_TEST) {
        // Test mode: just print key codes
        char msg[32];
        snprintf(msg, sizeof(msg), "Key pressed: %d\r\n", key);
        uart_send_string(msg);
      } else {
        // Application mode
        process_key(key);
      }
    }

    if (app_state.edit_mode) {
      uint8_t received_char;
      if (uart_rx_blocking(&received_char)) {
        if (received_char == '\r' || received_char == '\n') {
          app_state.input_buffer[app_state.input_index] = '\0';
          uart_send_string("\r\n");
          process_input_buffer();
          app_state.edit_mode = false;
          send_status_message();
        } else if (received_char == 8 || received_char == 127) {
          if (app_state.input_index > 0) {
            app_state.input_index--;
            uart_send_string("\b \b");
          }
        } else if (app_state.input_index < sizeof(app_state.input_buffer) - 1) {
          app_state.input_buffer[app_state.input_index++] = received_char;
          uart_tx_blocking(received_char);
        }
      }
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
static void process_key(uint8_t key_code) {
  char message[64];

  switch (key_code) {
  case 1:
  case 2:
  case 3:
  case 4:
    play_melody(key_code - 1);
    break;

  case 5:
    // Custom melody
    if (user_melody.note_count > 0) {
      melody_play(&user_melody);
      snprintf(message, sizeof(message), "\r\nPlaying custom melody\r\n");
      uart_send_string(message);
    } else {
      uart_send_string("\r\nNo custom melody defined\r\n");
    }
    break;

  case 6:
    // Enter edit mode
    enter_edit_mode();
    break;

  default:
    snprintf(message, sizeof(message), "\r\nKey %d has no function\r\n",
             key_code);
    uart_send_string(message);
    break;
  }
}

static void play_melody(uint8_t melody_index) {
  if (melody_index < MAX_STANDARD_MELODIES) {
    melody_play(&standard_melodies[melody_index]);
    char message[64];
    snprintf(message, sizeof(message), "\r\nPlaying melody %d: %s\r\n",
             melody_index + 1, standard_melodies[melody_index].name);
    uart_send_string(message);
  }
}

static void send_status_message(void) {
  char message[128];
  snprintf(message, sizeof(message), "\r\nStatus: Custom notes=%d\r\n> ",
           user_melody.note_count);
  uart_send_string(message);
}

static void enter_edit_mode(void) {
  uart_send_string("\r\n=== Edit Custom Melody ===\r\n");
  uart_send_string("Format: C4:200;D4:200;N:100;E4:400;\r\n");
  uart_send_string("Notes: C,D,E,F,G,A,B + octave (0-8)\r\n");
  uart_send_string("N = pause, :duration_ms\r\n");
  uart_send_string("Enter melody via UART (end with Enter):\r\n> ");
  app_state.edit_mode = true;
  app_state.input_index = 0;
}

static void process_input_buffer(void) {
  char message[64];
  if (parse_melody_string(app_state.input_buffer, &user_melody)) {
    snprintf(message, sizeof(message), "Melody parsed: %d notes\r\n",
             user_melody.note_count);
    uart_send_string(message);
  } else {
    uart_send_string("Error: Invalid format\r\n");
    uart_send_string("Example: C4:200;D4:200;N:100;E4:400;\r\n");
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
