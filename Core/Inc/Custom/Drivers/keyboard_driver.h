#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H

#include "stdint.h"

void keyboard_init(void);
void keyboard_update(void);
bool keyboard_key_pressed(void);
uint8_t keyboard_get_pressed_key(void);

#endif // KEYBOARD_DRIVER_H