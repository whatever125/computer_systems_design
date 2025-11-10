#include "custom_gpio.h"

#define GPIO_NUMBER_OF_PINS 16U

void custom_gpio_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // enable GPIO clocks
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  // set initial LED states to OFF
  custom_gpio_write_pin(GPIOD, Green_LED_Pin | Red_LED_Pin | Yellow_LED_Pin,
                        GPIO_PIN_RESET);

  // init button
  GPIO_InitStruct.Pin = Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  custom_gpio_init_pin(Button_GPIO_Port, GPIO_InitStruct.Pin, &GPIO_InitStruct);

  // init LEDs
  GPIO_InitStruct.Pin = Green_LED_Pin | Red_LED_Pin | Yellow_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  custom_gpio_init_pin(GPIOD, GPIO_InitStruct.Pin, &GPIO_InitStruct);
}

void custom_gpio_init_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,
                          GPIO_InitTypeDef* GPIO_InitStruct) {
  uint32_t position;
  uint32_t ioposition = 0x00U;
  uint32_t iocurrent = 0x00U;
  uint32_t temp = 0x00U;

  if ((GPIOx == NULL) || (GPIO_InitStruct == NULL) || (GPIO_Pin == 0)) {
    return;
  }

  // Handle every pin specified in GPIO_Pin
  for (position = 0U; position < GPIO_NUMBER_OF_PINS; position++) {
    ioposition = ((uint32_t)1U) << position;
    iocurrent = (GPIO_Pin & ioposition);

    if (iocurrent == ioposition) {
      // Configure IO Direction mode (Input, Output, ...)
      temp = GPIOx->MODER;
      temp &= ~(GPIO_MODER_MODER0 << (position * 2U));
      temp |= ((GPIO_InitStruct->Mode & GPIO_MODE) << (position * 2U));
      GPIOx->MODER = temp;

      // Output mode
      if ((GPIO_InitStruct->Mode == GPIO_MODE_OUTPUT_PP) ||
          (GPIO_InitStruct->Mode == GPIO_MODE_OUTPUT_OD)) {
        // Output type
        temp = GPIOx->OTYPER;
        temp &= ~(GPIO_OTYPER_OT_0 << position);
        temp |= (((GPIO_InitStruct->Mode & OUTPUT_TYPE) >> OUTPUT_TYPE_Pos)
                 << position);
        GPIOx->OTYPER = temp;

        // Speed
        temp = GPIOx->OSPEEDR;
        temp &= ~(GPIO_OSPEEDER_OSPEEDR0 << (position * 2U));
        temp |= (GPIO_InitStruct->Speed << (position * 2U));
        GPIOx->OSPEEDR = temp;
      }

      // Pull Up/Down
      if ((GPIO_InitStruct->Mode != GPIO_MODE_ANALOG)) {
        temp = GPIOx->PUPDR;
        temp &= ~(GPIO_PUPDR_PUPDR0 << (position * 2U));
        temp |= ((GPIO_InitStruct->Pull) << (position * 2U));
        GPIOx->PUPDR = temp;
      }
    }
  }
}

uint32_t custom_gpio_read_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
  GPIO_PinState bitstatus;
  if ((GPIOx->IDR & GPIO_Pin) != (uint32_t)GPIO_PIN_RESET) {
    bitstatus = GPIO_PIN_SET;
  } else {
    bitstatus = GPIO_PIN_RESET;
  }
  return bitstatus;
}

void custom_gpio_write_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,
                           GPIO_PinState PinState) {
  if (PinState != (GPIO_PinState)GPIO_PIN_RESET) {
    GPIOx->BSRR = GPIO_Pin;
  } else {
    GPIOx->BSRR = (GPIO_Pin << 16U);
  }
}

void custom_gpio_toggle_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
  GPIOx->ODR ^= GPIO_Pin;
}
