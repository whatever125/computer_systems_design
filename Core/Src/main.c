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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ctype.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "Custom/Drivers/led_driver.h"
#include "Custom/Drivers/button_driver.h"
#include "Custom/Drivers/uart_driver.h"
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
static uart_mode_t current_mode = UART_POLLING;

static enum calc_state_t {
  START,
  NUM1,
  GOT_OP,
} state = START;

static char num1_str[6] = {0}, num2_str[6] = {0};
static char op = 0;
static int len1 = 0, len2 = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void reset_calc_state(void) {
  state = START;
  len1 = len2 = 0;
  num1_str[0] = num2_str[0] = op = 0;
}

static void prompt_for_expression(void) {
  uart_send_string("> ");
}

static void reset_on_mode_switch(void) {
  reset_calc_state();
  uart_send_string("\r\nSwitched to ");
  if (current_mode == UART_POLLING) {
    uart_send_string("polling mode\r\n");
  } else {
    uart_send_string("interrupt mode\r\n");
  }
  led_set_state(RED_LED, LED_OFF);
  prompt_for_expression();
}

static void calc_error(const char* msg) {
  led_set_state(RED_LED, LED_ON);
  uart_send_string("\r\nerror");
  if (msg) {
    uart_send_string(": ");
    uart_send_string(msg);
  }
  uart_send_string("\r\n");
  reset_calc_state();
  prompt_for_expression();
}

static void compute_result(void) {
  int raw_n1 = atoi(num1_str);
  int raw_n2 = atoi(num2_str);
  if (raw_n1 < 0 || raw_n1 > 32767) {
    calc_error("Invalid num1");
    return;
  }
  if (raw_n2 < 0 || raw_n2 > 32767) {
    calc_error("Invalid num2");
    return;
  }

  short n1 = (short)raw_n1;
  short n2 = (short)raw_n2;

  short res = 0;
  bool err = false;

  switch (op) {
  case '+':
    if ((long)n1 + n2 > 32767)
      err = true;
    else
      res = n1 + n2;
    break;

  case '-':
    res = n1 - n2;
    break;

  case '*':
    if ((long)n1 * n2 > 32767 || (long)n1 * n2 < -32768)
      err = true;
    else
      res = n1 * n2;
    break;

  case '/':
    if (n2 == 0) {
      calc_error("Div by 0");
      return;
    }
    res = n1 / n2;
    break;

  default:
    calc_error("Invalid op");
    return;
  }

  if (err) {
    calc_error("Overflow");
    return;
  }

  char buf[12];
  sprintf(buf, "%d\r\n", res);
  uart_send_string(buf);
  led_set_state(RED_LED, LED_OFF);
  reset_calc_state();
  prompt_for_expression();
}

static void process_char(uint8_t ch) {
  if (current_mode == UART_POLLING) {
    uart_tx_blocking(ch);
  } else {
    uart_tx_nonblocking(ch);
  }

  if (ch < '0' || ch > '9') {
    if (ch != '+' && ch != '-' && ch != '*' && ch != '/' && ch != '=') {
      calc_error("Invalid char");
      return;
    }
  }

  switch (state) {
  case START:
  case NUM1:
    if (isdigit(ch)) {
      if (len1 < 5) {
        num1_str[len1++] = ch;
        num1_str[len1] = 0;
        state = NUM1;
      } else {
        calc_error("Num1 too long");
      }
    } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
      if (len1 == 0) {
        calc_error("No num1");
        return;
      }
      op = ch;
      state = GOT_OP;
      len2 = 0;
      num2_str[0] = 0;
    } else if (ch == '=') {
      calc_error("No expression");
    }
    break;

  case GOT_OP:
    if (isdigit(ch)) {
      if (len2 < 5) {
        num2_str[len2++] = ch;
        num2_str[len2] = 0;
      } else {
        calc_error("Num2 too long");
      }
    } else if (ch == '=') {
      if (len2 == 0) {
        calc_error("No num2");
        return;
      }
      compute_result();
    } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
      calc_error("Extra operator");
      return;
    }
    break;
  }
}
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
  /* USER CODE BEGIN 2 */
  button_driver_init();
  uart_driver_init(current_mode);
  uart_send_string("\r\n\r\n\
▓█████▄  ███▄    █  ▄▄▄▄   \r\n\
▒██▀ ██▌ ██ ▀█   █ ▓█████▄ \r\n\
░██   █▌▓██  ▀█ ██▒▒██▒ ▄██\r\n\
░▓█▄   ▌▓██▒  ▐▌██▒▒██░█▀  \r\n\
░▒████▓ ▒██░   ▓██░░▓█  ▀█▓\r\n\
 ▒▒▓  ▒ ░ ▒░   ▒ ▒ ░▒▓███▀▒\r\n\
 ░ ▒  ▒ ░ ░░   ░ ▒░▒░▒   ░ \r\n\
 ░ ░  ░    ░   ░ ░  ░    ░ \r\n\
   ░             ░  ░      \r\n\
 ░                       ░ \r\n\r\n");
  reset_on_mode_switch();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    button_update_state();
    press_type_t current_press = button_get_press_type();

    if (current_press == SHORT_PRESS) {
      if (current_mode == UART_POLLING) {
        // switch to interrupt mode
        current_mode = UART_INTERRUPT;
        uart_switch_mode(UART_INTERRUPT);
      } else {
        // switch to polling mode
        current_mode = UART_POLLING;
        uart_switch_mode(UART_POLLING);
      }
      reset_on_mode_switch();
    }

    uint8_t ch;
    if (current_mode == UART_INTERRUPT) {
      while (uart_has_rx_data()) {
        ch = uart_get_rx_data();
        process_char(ch);
      }
    } else {
      if (uart_rx_blocking(&ch)) {
        process_char(ch);
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
