#include "main.h"
#include "Custom/Drivers/led_driver.h"
#include "gpio.h"
#include "stm32f427xx.h"

static GPIO_TypeDef* led_ports[LED_COUNT] = {
    Red_LED_GPIO_Port,
    Yellow_LED_GPIO_Port,
};
static uint16_t led_pins[LED_COUNT] = {
    Red_LED_Pin,
    Yellow_LED_Pin,
};

/**
 * Set LED state
 * @param led_id: LED ID
 * @param state: LED ON/OFF
 */
void led_set_state(led_id_t led_id, led_state_t state) {
  if (led_id < LED_COUNT) {
    HAL_GPIO_WritePin(led_ports[led_id], led_pins[led_id],
                      (state == LED_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
}

/**
 * Toggle LED state
 * @param led_id: LED ID
 */
void led_toggle(led_id_t led_id) {
  if (led_id < LED_COUNT) {
    HAL_GPIO_TogglePin(led_ports[led_id], led_pins[led_id]);
  }
}