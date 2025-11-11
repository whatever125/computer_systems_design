#include "Custom/Drivers/button_driver.h"
#include "gpio.h"
#include "main.h"

#include "stdbool.h"
#include "stdint.h"

#include "stm32f427xx.h"

#define BUTTON_PRESSED GPIO_PIN_RESET
#define BUTTON_RELEASED GPIO_PIN_SET

#define BUTTON_GPIO_PORT Button_GPIO_Port
#define BUTTON_PIN Button_Pin

static GPIO_PinState last_button_state = false;
static GPIO_PinState current_state = false;
static uint32_t last_state_change_tick = 0;
static bool press_detected = false;
static press_type_t press_type = NO_PRESS;
static uint32_t press_start_tick = 0;

/**
 * Init button driver
 */
void button_driver_init(void) {
  // init states
  last_button_state = HAL_GPIO_ReadPin(BUTTON_GPIO_PORT, BUTTON_PIN);
  current_state = last_button_state;
  last_state_change_tick = HAL_GetTick();
}

/**
 * Update button state
 */
void button_update_state(void) {
  // update state
  uint32_t current_tick = HAL_GetTick();
  current_state = HAL_GPIO_ReadPin(BUTTON_GPIO_PORT, BUTTON_PIN);

  if (current_state != last_button_state &&
      (current_tick - last_state_change_tick) >= DEBOUNCE_TIME_MS) {
    // state changed, debounce timer passed
    last_button_state = current_state;
    last_state_change_tick = current_tick;

    if (last_button_state == BUTTON_PRESSED) {
      // button pressed, reset state and start timer
      press_start_tick = current_tick;
      press_detected = false;
      press_type = NO_PRESS;
    } else {
      // button released, determine press type
      uint32_t press_duration = current_tick - press_start_tick;
      if (press_duration < LONG_PRESS_THRESHOLD_MS) {
        press_type = SHORT_PRESS;
      } else {
        press_type = LONG_PRESS;
      }
      press_detected = true;
    }
  }
}

/**
 * Get press type and reset state
 */
press_type_t button_get_press_type(void) {
  press_type_t temp = press_type;
  if (press_type != NO_PRESS) {
    press_type = NO_PRESS;
    press_detected = false;
  }
  return temp;
}