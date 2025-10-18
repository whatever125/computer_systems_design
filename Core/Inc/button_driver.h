#ifndef __BUTTON_DRIVER_H
#define __BUTTON_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum {
  NO_PRESS,
  SHORT_PRESS,
  LONG_PRESS,
} button_press_type_t;

void button_driver_init(void);
void button_update_state(void);
button_press_type_t button_get_press_type(void);

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_DRIVER_H */