#ifndef MELODY_DRIVER_H
#define MELODY_DRIVER_H

#include "stdint.h"
#include "stdbool.h"

#define MAX_MELODY_LENGTH 128
#define MAX_STANDARD_MELODIES 4

typedef struct {
  uint32_t frequency;
  uint32_t duration;
} note_t;

typedef struct {
  note_t notes[MAX_MELODY_LENGTH];
  uint16_t note_count;
  char name[16];
} melody_t;

extern melody_t standard_melodies[MAX_STANDARD_MELODIES];
extern melody_t user_melody;

void melody_driver_init(void);
void melody_play(const melody_t* melody);
void melody_stop(void);
bool melody_is_playing(void);
void melody_update(void);

#endif