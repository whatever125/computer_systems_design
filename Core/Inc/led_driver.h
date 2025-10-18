#ifndef __LED_DRIVER_H
#define __LED_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum {
  LED_OFF,
  LED_ON,
} led_state_t;

typedef enum {
  GREEN_LED,
  YELLOW_LED,
  LED_COUNT,
} led_id_t;

void led_driver_init(void);
void led_set_state(led_id_t led_id, led_state_t state);
void led_toggle(led_id_t led_id);

#ifdef __cplusplus
}
#endif

#endif /* __LED_DRIVER_H */