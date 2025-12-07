#ifndef SOUND_DRIVER_H
#define SOUND_DRIVER_H

#include "stdint.h"
#include "stdbool.h"

void sound_driver_init(void);
void sound_play_frequency(uint32_t freq_hz);
void sound_stop(void);

#endif