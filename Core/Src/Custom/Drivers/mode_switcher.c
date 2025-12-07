#include "Custom/Drivers/mode_switcher.h"
#include "Custom/Drivers/button_driver.h"
#include "Custom/Drivers/uart_driver.h"

static app_mode_t current_mode = MODE_TEST;

void mode_switcher_init(void) {
  button_driver_init();
  uart_send_string("Keyboard Test Mode\r\n");
  uart_send_string("Press side button to switch modes\r\n");
}

void mode_switcher_update(void) {
  static bool last_button_state = false;

  button_update_state();
  press_type_t press = button_get_press_type();

  if (press == SHORT_PRESS) {
    current_mode = (current_mode == MODE_TEST) ? MODE_APPLICATION : MODE_TEST;

    if (current_mode == MODE_TEST) {
      uart_send_string("\r\n=== Keyboard Test Mode ===\r\n");
      uart_send_string("Press keys to see their codes (1-12)\r\n");
    } else {
      uart_send_string("\r\n=== Application Mode ===\r\n");
      uart_send_string("Keys 1-4: Play melodies, 5: Custom, 6: Edit\r\n");
    }
  }
}

app_mode_t get_current_mode(void) {
  return current_mode;
}