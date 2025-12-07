#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H

#include "stdbool.h"
#include "stdint.h"

void keyboard_init(void);
bool keyboard_has_event(void);
uint8_t keyboard_get_pressed_key(void);

#endif // KEYBOARD_DRIVER_H