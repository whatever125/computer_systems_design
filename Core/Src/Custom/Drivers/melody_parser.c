#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "Custom/Drivers/melody_parser.h"

static const uint32_t note_frequencies[7] = {262, 294, 330, 349, 392, 440, 494};
static const char* note_names = "CDEFGAB";

static uint32_t get_note_frequency(char note, int octave) {
  note = toupper(note);

  const char* pos = strchr(note_names, note);
  if (pos == NULL)
    return 0;

  int note_index = pos - note_names;
  if (note_index < 0 || note_index >= 7)
    return 0;

  int octave_diff = octave - 4;
  uint32_t freq = note_frequencies[note_index];

  if (octave_diff > 0) {
    freq <<= octave_diff;
  } else if (octave_diff < 0) {
    freq >>= (-octave_diff);
  }

  return freq;
}

bool parse_melody_string(const char* str, melody_t* melody) {
  if (str == NULL || melody == NULL)
    return false;

  char buffer[128];
  strncpy(buffer, str, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';

  melody->note_count = 0;

  char* token = strtok(buffer, ";");
  while (token != NULL && melody->note_count < MAX_MELODY_LENGTH) {
    if (*token == '\0') {
      token = strtok(NULL, ";");
      continue;
    }

    if (toupper(*token) == 'N') {
      melody->notes[melody->note_count].frequency = 0;
      token++;
    } else {
      char note = *token;
      token++;

      if (!isdigit((unsigned char)*token)) {
        return false;
      }

      int octave = *token - '0';
      if (octave < 0 || octave > 8) {
        return false;
      }

      melody->notes[melody->note_count].frequency =
          get_note_frequency(note, octave);
      token++;
    }

    if (*token != ':') {
      return false;
    }
    token++;

    char* endptr;
    uint32_t duration = strtoul(token, &endptr, 10);
    if (duration == 0 || duration > 10000) {
      return false;
    }

    melody->notes[melody->note_count].duration = duration;
    melody->note_count++;

    token = strtok(NULL, ";");
  }

  return melody->note_count > 0;
}

void clear_user_melody(void) {
  user_melody.note_count = 0;
}
