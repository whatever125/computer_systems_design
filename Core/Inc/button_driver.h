#ifndef __BUTTON_DRIVER_H
#define __BUTTON_DRIVER_H

#include "main.h"

typedef enum {
  NO_PRESS,
  SHORT_PRESS,
  LONG_PRESS,
} press_type_t;

void button_driver_init(void);
void button_update_state(void);
press_type_t button_get_press_type(void);

#endif /* __BUTTON_DRIVER_H */