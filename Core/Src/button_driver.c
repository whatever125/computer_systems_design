#include "button_driver.h"
#include "gpio.h"
#include "main.h"

#include "stdbool.h"
#include "stdint.h"

#include "stm32f427xx.h"

#define PRESSED_STATE GPIO_PIN_RESET

static bool last_button_state = false;
static bool current_raw_state = false;
static uint32_t last_state_change_tick = 0;
static bool press_detected = false;
static button_press_type_t pending_press = NO_PRESS;
static uint32_t press_start_tick = 0;

/**
 * @brief Инициализация драйвера кнопки (GPIO уже в MX_GPIO_Init)
 */
void button_driver_init(void) {
  last_button_state =
      HAL_GPIO_ReadPin(Button_GPIO_Port, Button_Pin) == PRESSED_STATE;
  current_raw_state = last_button_state;
  last_state_change_tick = HAL_GetTick();
}

/**
 * @brief Обновить состояние кнопки (вызывать в цикле, неблокирующе)
 * Проверяет debounce и детектирует press/release.
 */
void button_update_state(void) {
  uint32_t current_tick = HAL_GetTick();
  current_raw_state =
      (HAL_GPIO_ReadPin(Button_GPIO_Port, Button_Pin) == PRESSED_STATE);

  /* Debounce: игнорируем изменения, если < DEBOUNCE_TIME_MS */
  if (current_raw_state != last_button_state) {
    if ((current_tick - last_state_change_tick) >= DEBOUNCE_TIME_MS) {
      /* Стабильное изменение */
      last_button_state = current_raw_state;
      last_state_change_tick = current_tick;

      if (last_button_state) {
        /* Нажатие: начать измерение длительности */
        press_start_tick = current_tick;
        press_detected = false;
        pending_press = NO_PRESS;
      } else {
        /* Отпускание: определить тип по длительности */
        if (!press_detected) {
          uint32_t press_duration = current_tick - press_start_tick;
          if (press_duration < SHORT_PRESS_THRESHOLD_MS) {
            pending_press = SHORT_PRESS;
          } else if (press_duration >= LONG_PRESS_THRESHOLD_MS) {
            pending_press = LONG_PRESS;
          }
          press_detected = true;
        }
      }
    }
  } else {
    /* Состояние стабильно, проверяем long press в реальном времени */
    if (last_button_state && press_detected) {
      uint32_t elapsed_press = current_tick - press_start_tick;
      if (elapsed_press >= LONG_PRESS_THRESHOLD_MS &&
          pending_press == NO_PRESS) {
        pending_press = LONG_PRESS;
        press_detected = true; /* Чтобы не менять дальше */
      }
    }
  }
}

/**
 * @brief Получить тип нажатия (SHORT/LONG или NO)
 * @note: Вызывать после button_update_state(); сбрасывает флаг после чтения.
 */
button_press_type_t button_get_press_type(void) {
  button_press_type_t press_type = pending_press;
  if (press_type != NO_PRESS) {
    pending_press = NO_PRESS; /* Сброс для следующего */
    press_detected = false;
  }
  return press_type;
}