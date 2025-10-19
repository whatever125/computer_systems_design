#ifndef __CUSTOM_GPIO_H
#define __CUSTOM_GPIO_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f427xx.h"

#define Button_Pin GPIO_PIN_15
#define Button_GPIO_Port GPIOC
#define Green_LED_Pin GPIO_PIN_13
#define Green_LED_GPIO_Port GPIOD
#define Red_LED_Pin GPIO_PIN_14
#define Red_LED_GPIO_Port GPIOD
#define Yellow_LED_Pin GPIO_PIN_15
#define Yellow_LED_GPIO_Port GPIOD

void custom_gpio_init(void);
void custom_gpio_init_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,
                          GPIO_InitTypeDef* GPIO_InitStruct);
uint32_t custom_gpio_read_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void custom_gpio_write_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,
                           GPIO_PinState PinState);
void custom_gpio_toggle_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif /* __CUSTOM_GPIO_H */