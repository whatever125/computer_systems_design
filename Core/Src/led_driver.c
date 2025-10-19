#include "custom_gpio.h"
#include "led_driver.h"

static GPIO_TypeDef* led_ports[LED_COUNT] = {
    Green_LED_GPIO_Port,
    Red_LED_GPIO_Port,
    Yellow_LED_GPIO_Port,
};
static uint16_t led_pins[LED_COUNT] = {
    Green_LED_Pin,
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
    custom_gpio_write_pin(led_ports[led_id], led_pins[led_id],
                          (state == LED_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
}

/**
 * Toggle LED state
 * @param led_id: LED ID
 */
void led_toggle(led_id_t led_id) {
  if (led_id < LED_COUNT) {
    custom_gpio_toggle_pin(led_ports[led_id], led_pins[led_id]);
  }
}