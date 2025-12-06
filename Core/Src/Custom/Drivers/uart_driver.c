#include "usart.h"
#include "Custom/Drivers/uart_driver.h"

#define BUF_SIZE 128

static uart_mode_t current_mode = UART_POLLING;

static uint8_t rx_buf[BUF_SIZE];
static volatile uint16_t rx_head = 0, rx_tail = 0;
static uint8_t tx_buf[BUF_SIZE];
static volatile uint16_t tx_head = 0, tx_tail = 0;
static bool tx_in_progress = false;

static void reset_buf(void) {
  rx_head = rx_tail = 0;
  tx_head = tx_tail = 0;
  tx_in_progress = false;
}

void uart_driver_init(uart_mode_t mode) {
  current_mode = mode;
  reset_buf();
  uart_switch_mode(mode);
  if (mode == UART_INTERRUPT) {
    HAL_UART_Receive_IT(&huart6, rx_buf, 1);
  }
}

void uart_switch_mode(uart_mode_t new_mode) {
  if (new_mode == current_mode)
    return;
  HAL_UART_Abort(&huart6);
  HAL_NVIC_DisableIRQ(USART6_IRQn);
  reset_buf();
  if (new_mode == UART_INTERRUPT) {
    HAL_NVIC_EnableIRQ(USART6_IRQn);
    HAL_UART_Receive_IT(&huart6, rx_buf, 1);
  }
  current_mode = new_mode;
}

uart_mode_t uart_get_mode(void) {
  return current_mode;
}

bool uart_rx_blocking(uint8_t* data) {
  if (current_mode != UART_POLLING)
    return false;
  uint8_t temp;
  if (HAL_UART_Receive(&huart6, &temp, 1, 1) == HAL_OK) {
    *data = temp;
    return true;
  }
  return false;
}

void uart_tx_blocking(uint8_t data) {
  if (current_mode != UART_POLLING)
    return;
  HAL_UART_Transmit(&huart6, &data, 1, HAL_MAX_DELAY);
}

bool uart_tx_nonblocking(uint8_t data) {
  if (current_mode != UART_INTERRUPT)
    return false;
  uint32_t pmask = __get_PRIMASK();
  __disable_irq();
  uint16_t next_head = (tx_head + 1) % BUF_SIZE;
  if (next_head == tx_tail) {
    __set_PRIMASK(pmask);
    return false;
  }
  bool was_empty = (tx_head == tx_tail);
  tx_buf[tx_head] = data;
  tx_head = next_head;
  if (was_empty && !tx_in_progress) {
    uint8_t* ptr = &tx_buf[tx_tail];
    HAL_UART_Transmit_IT(&huart6, ptr, 1);
    tx_tail = (tx_tail + 1) % BUF_SIZE;
    tx_in_progress = true;
  }
  __set_PRIMASK(pmask);
  return true;
}

static bool uart_send_string_nonblocking_internal(const char* str,
                                                  bool* sent_all) {
  *sent_all = true;
  const char* p = str;
  while (*p) {
    if (!uart_tx_nonblocking(*p)) {
      *sent_all = false;
      return false;
    }
    p++;
  }
  return true;
}

void uart_send_string(const char* str) {
  if (current_mode == UART_POLLING) {
    while (*str) {
      uart_tx_blocking(*str++);
    }
  } else {
    bool sent_all;
    do {
      uart_send_string_nonblocking_internal(str, &sent_all);
      if (!sent_all) {
        HAL_Delay(1);
      }
    } while (!sent_all);
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
  if (current_mode != UART_INTERRUPT)
    return;
  uint8_t temp;
  temp = rx_buf[0];
  uint16_t next_head = (rx_head + 1) % BUF_SIZE;
  if (next_head != rx_tail) {
    rx_buf[rx_head] = temp;
    rx_head = next_head;
  }
  HAL_UART_Receive_IT(huart, rx_buf, 1);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart) {
  if (current_mode != UART_INTERRUPT)
    return;
  if (tx_head != tx_tail) {
    uint8_t* ptr = &tx_buf[tx_tail];
    HAL_UART_Transmit_IT(huart, ptr, 1);
    tx_tail = (tx_tail + 1) % BUF_SIZE;
  } else {
    tx_in_progress = false;
  }
}

bool uart_has_rx_data(void) {
  uint32_t pmask = __get_PRIMASK();
  __disable_irq();
  bool has = (rx_head != rx_tail);
  __set_PRIMASK(pmask);
  return has;
}

uint8_t uart_get_rx_data(void) {
  uint32_t pmask = __get_PRIMASK();
  __disable_irq();
  if (!uart_has_rx_data()) {
    __set_PRIMASK(pmask);
    return 0;
  }
  uint8_t c = rx_buf[rx_tail];
  rx_tail = (rx_tail + 1) % BUF_SIZE;
  __set_PRIMASK(pmask);
  return c;
}
