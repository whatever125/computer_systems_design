#include "Custom/Drivers/keyboard_driver.h"
#include "i2c.h"
#include "main.h"
#include "stdbool.h"
#include "stdint.h"

#define KEYBOARD_I2C_ADDR 0xE2
#define KEY_PRESSED_THRESHOLD 5
#define KEY_RELEASED_THRESHOLD 2

static const uint8_t rows[4] = {0x01, 0x02, 0x04, 0x08};
static const uint8_t col_masks[3] = {0x10, 0x20, 0x40};

static uint8_t key_states[12] = {0};
static uint8_t pressed_keys[12] = {0};
static uint8_t prev_pressed[12] = {0};
static uint8_t key_codes[12] = {0};

void keyboard_init(void) {
  for (uint8_t i = 0; i < 12; i++) {
    key_codes[i] = i + 1;
  }
  uint8_t config = 0x70;
  HAL_I2C_Mem_Write(&hi2c1, KEYBOARD_I2C_ADDR, 0x03, 1, &config, 1,
                    HAL_MAX_DELAY);
}

uint8_t scan_keyboard(void) {
  uint8_t result = 0;

  for (uint8_t row_idx = 0; row_idx < 4; row_idx++) {
    uint8_t row_config = rows[row_idx];
    HAL_I2C_Mem_Write(&hi2c1, KEYBOARD_I2C_ADDR, 0x03, 1, &row_config, 1,
                      HAL_MAX_DELAY);

    uint8_t column_data = 0;
    HAL_I2C_Mem_Read(&hi2c1, KEYBOARD_I2C_ADDR, 0x00, 1, &column_data, 1,
                     HAL_MAX_DELAY);

    for (uint8_t col_idx = 0; col_idx < 3; col_idx++) {
      uint8_t key_index = row_idx * 3 + col_idx;

      if (!(column_data & col_masks[col_idx])) {
        if (key_states[key_index] < KEY_PRESSED_THRESHOLD) {
          key_states[key_index]++;
        }
      } else {
        if (key_states[key_index] > 0) {
          key_states[key_index]--;
        }
      }

      if (key_states[key_index] >= KEY_PRESSED_THRESHOLD) {
        pressed_keys[key_index] = 1;
      } else if (key_states[key_index] <= KEY_RELEASED_THRESHOLD) {
        pressed_keys[key_index] = 0;
      }
    }
  }

  uint8_t default_config = 0x70;
  HAL_I2C_Mem_Write(&hi2c1, KEYBOARD_I2C_ADDR, 0x03, 1, &default_config, 1,
                    HAL_MAX_DELAY);

  return result;
}

bool keyboard_has_event(void) {
  scan_keyboard();
  for (uint8_t i = 0; i < 12; i++) {
    if (pressed_keys[i] && !prev_pressed[i]) {
      prev_pressed[i] = 1;
      return true;
    } else if (!pressed_keys[i] && prev_pressed[i]) {
      prev_pressed[i] = 0;
    }
  }
  return false;
}

uint8_t keyboard_get_pressed_key(void) {
  for (uint8_t i = 0; i < 12; i++) {
    if (pressed_keys[i] && !prev_pressed[i]) {
      prev_pressed[i] = 1;
      return key_codes[i];
    }
  }
  return 0;
}