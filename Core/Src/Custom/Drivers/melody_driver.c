#include "stdio.h"
#include "main.h"
#include "Custom/Drivers/melody_driver.h"
#include "Custom/Drivers/sound_driver.h"

melody_t standard_melodies[MAX_STANDARD_MELODIES];
melody_t user_melody = {0};

static const melody_t default_melodies[MAX_STANDARD_MELODIES] = {
    {
        .notes = {{262, 200},
                  {294, 200},
                  {330, 200},
                  {349, 200},
                  {392, 200},
                  {440, 200},
                  {494, 200},
                  {523, 400},
                  {0, 100}},
        .note_count = 9,
        .name = "Scale",
    },
    {
        .notes = {{392, 300},
                  {440, 300},
                  {494, 300},
                  {523, 600},
                  {494, 300},
                  {440, 300},
                  {392, 600},
                  {0, 100}},
        .note_count = 8,
        .name = "Joy",
    },
    {
        .notes = {{880, 100},
                  {0, 100},
                  {880, 100},
                  {0, 100},
                  {880, 100},
                  {0, 100},
                  {880, 100},
                  {0, 300},
                  {660, 100},
                  {0, 100},
                  {660, 100},
                  {0, 100},
                  {660, 100},
                  {0, 100},
                  {660, 100},
                  {0, 300}},
        .note_count = 16,
        .name = "Alarm",
    },
    {
        .notes = {{392, 200},
                  {392, 200},
                  {440, 200},
                  {494, 200},
                  {494, 200},
                  {440, 200},
                  {392, 200},
                  {330, 200},
                  {294, 200},
                  {294, 200},
                  {330, 200},
                  {392, 200},
                  {392, 300},
                  {330, 100},
                  {330, 400},
                  {0, 200}},
        .note_count = 16,
        .name = "Rickroll",
    },
};

static struct {
  const melody_t* current_melody;
  uint16_t current_note;
  uint32_t note_start_time;
  bool playing;
} playback_state = {0};

void melody_driver_init(void) {
  for (int i = 0; i < MAX_STANDARD_MELODIES; i++) {
    standard_melodies[i] = default_melodies[i];
  }

  user_melody.note_count = 0;
  snprintf(user_melody.name, sizeof(user_melody.name), "Custom");
}

void melody_play(const melody_t* melody) {
  if (melody == NULL || melody->note_count == 0)
    return;

  playback_state.current_melody = melody;
  playback_state.current_note = 0;
  playback_state.note_start_time = HAL_GetTick();
  playback_state.playing = true;

  sound_play_frequency(melody->notes[0].frequency);
}

void melody_stop(void) {
  playback_state.playing = false;
  sound_stop();
}

bool melody_is_playing(void) {
  return playback_state.playing;
}

void melody_update(void) {
  if (!playback_state.playing || playback_state.current_melody == NULL)
    return;

  uint32_t current_time = HAL_GetTick();
  uint32_t elapsed = current_time - playback_state.note_start_time;

  const note_t* current_note =
      &playback_state.current_melody->notes[playback_state.current_note];

  if (elapsed >= current_note->duration) {
    playback_state.current_note++;

    if (playback_state.current_note >=
        playback_state.current_melody->note_count) {
      melody_stop();
      return;
    }

    playback_state.note_start_time = current_time;
    sound_play_frequency(
        playback_state.current_melody->notes[playback_state.current_note]
            .frequency);
  }
}
