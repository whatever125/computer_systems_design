#ifndef MELODY_PARSER_H
#define MELODY_PARSER_H

#include "stdint.h"
#include "stdbool.h"
#include "melody_driver.h"

bool parse_melody_string(const char* str, melody_t* melody);
void clear_user_melody(void);

#endif