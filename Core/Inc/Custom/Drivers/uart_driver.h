#ifndef __UART_DRIVER_H
#define __UART_DRIVER_H

#include "usart.h"
#include "stdbool.h"
#include "stdint.h"

typedef enum {
  UART_POLLING,
  UART_INTERRUPT,
} uart_mode_t;

void uart_driver_init(uart_mode_t mode);
void uart_switch_mode(uart_mode_t new_mode);
uart_mode_t uart_get_mode(void);

bool uart_rx_blocking(uint8_t* data);
bool uart_has_rx_data(void);
uint8_t uart_get_rx_data(void);

void uart_tx_blocking(uint8_t data);
bool uart_tx_nonblocking(uint8_t data);
void uart_send_string(const char* str);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart);

#endif /* __UART_DRIVER_H */
