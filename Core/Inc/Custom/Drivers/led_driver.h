#ifndef __LED_DRIVER_H
#define __LED_DRIVER_H

typedef enum {
  RED_LED,
  LED_COUNT,
} led_id_t;

typedef enum {
  LED_OFF,
  LED_ON,
} led_state_t;

void led_set_state(led_id_t led_id, led_state_t state);
void led_toggle(led_id_t led_id);

#endif /* __LED_DRIVER_H */