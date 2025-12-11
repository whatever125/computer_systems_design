#include "main.h"
#include "stdbool.h"
#include "stdint.h"
#include "Custom/Drivers/internal_keyboard_driver.h"
#include "Custom/Drivers/keyboard_driver.h"

#define DEBOUNCE_TIME_MS 20

static int8_t prev_raw_key_state;
static int8_t key_state;
static int8_t pressed_key;
static uint32_t last_change_time;
static bool key_event;

void keyboard_init(void) {
  prev_raw_key_state = -1;
  key_state = -1;
  pressed_key = -1;
  last_change_time = 0;
  key_event = false;
}

void keyboard_update(void) {
  int8_t raw_key_state = getKeyPressed();

  if (raw_key_state != prev_raw_key_state) {
    last_change_time = HAL_GetTick();
    prev_raw_key_state = raw_key_state;
  }

  if ((HAL_GetTick() - last_change_time) >= DEBOUNCE_TIME_MS) {
    if (raw_key_state != key_state) {
      key_state = raw_key_state;
      if (key_state >= 0 && pressed_key == -1) {
        pressed_key = key_state;
        key_event = true;
      } else if (key_state == -1) {
        pressed_key = -1;
      }
    }
  }
}

bool keyboard_key_pressed(void) {
  if (key_event) {
    key_event = false;
    return true;
  }
  return false;
}

uint8_t keyboard_get_pressed_key(void) {
  return (uint8_t)pressed_key;
}