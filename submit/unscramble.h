#ifndef UNSCRAMBLE_H
#define UNSCRAMBLE_H

#include <stdint.h>
#include <stdio.h>
#include "word.h"

struct Word_List unscramble_clear(uint8_t* data, uint16_t num_bytes);
struct Word_List unscramble_fuzzy(uint8_t* data, uint16_t num_bytes);

#endif
