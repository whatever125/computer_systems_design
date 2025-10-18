#include "led_driver.h"
#include "gpio.h"
#include "stm32f427xx.h"

static GPIO_TypeDef* led_ports[LED_COUNT] = {
    Green_LED_GPIO_Port,
    Yellow_LED_GPIO_Port,
};
static uint16_t led_pins[LED_COUNT] = {
    Green_LED_Pin,
    Yellow_LED_Pin,
};

/**
 * @brief Инициализация драйвера LED (настройка GPIO уже в MX_GPIO_Init)
 */
void led_driver_init(void) {
  /* Ничего дополнительного, GPIO уже настроены */
}

/**
 * @brief Установить состояние LED
 * @param led_id: ID LED
 * @param state: LED_ON/OFF
 */
void led_set_state(led_id_t led_id, led_state_t state) {
  if (led_id < LED_COUNT) {
    HAL_GPIO_WritePin(led_ports[led_id], led_pins[led_id],
                      (state == LED_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
}

/**
 * @brief Переключить состояние LED
 */
void led_toggle(led_id_t led_id) {
  if (led_id < LED_COUNT) {
    HAL_GPIO_TogglePin(led_ports[led_id], led_pins[led_id]);
  }
}