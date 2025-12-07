#ifndef MODE_SWITCHER_H
#define MODE_SWITCHER_H

#include "stdbool.h"
#include "stdint.h"

typedef enum { MODE_TEST, MODE_APPLICATION } app_mode_t;

void mode_switcher_init(void);
void mode_switcher_update(void);
app_mode_t get_current_mode(void);

#endif // MODE_SWITCHER_H