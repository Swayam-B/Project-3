#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "word.h"
#include "unscramble.h"

static uint16_t read_u16(uint8_t* ptr)
{
    return *( (uint16_t*)ptr );
}

struct Word_List unscramble_clear(uint8_t* data, uint16_t num_bytes)
{
    (void)num_bytes;

    uint16_t num_words = read_u16(data);
    uint16_t record_offset = read_u16(data + 2);

    struct Word_List list;
    init_list(&list, num_words);

    uint16_t count = 0;

    while (count < num_words)
    {
        uint8_t rec_len = *(data + record_offset);
        uint16_t next_record_offset = read_u16(data + record_offset + 1);

        uint16_t letters_offset = record_offset + 3;
        uint16_t letters_count = (uint16_t)rec_len;

        if (letters_count >= 3)
            letters_count = letters_count - 3;
        else
            letters_count = 0;

        char* temp = (char*)malloc(letters_count + 1);
        if (temp == NULL)
            break;

        uint16_t i = 0;
        while (i < letters_count)
        {
            *(temp + i) = (char)*(data + letters_offset + i);
            i++;
        }
        *(temp + letters_count) = '\0';

        struct Word w;
        init_word(&w);
        update_word(&w, temp, letters_offset);
        free(temp);

        add_word(&list, w);

        record_offset = next_record_offset;
        count++;
    }

    return list;
}

static uint8_t flip_nibbles(uint8_t x)
{
    uint8_t hi = (uint8_t)(x >> 4);
    uint8_t lo = (uint8_t)(x & 0x0F);
    return (uint8_t)((lo << 4) | hi);
}

struct Word_List unscramble_fuzzy(uint8_t* data, uint16_t num_bytes)
{
    (void)num_bytes;

    uint16_t num_words = read_u16(data);
    uint16_t record_offset = read_u16(data + 2);

    struct Word_List list;
    init_list(&list, num_words);

    uint16_t count = 0;

    while (count < num_words)
    {
        uint8_t rec_len = *(data + record_offset);
        uint16_t fuzzed_next = read_u16(data + record_offset + 1);

        uint16_t letters_offset = record_offset + 3;
        uint16_t letters_count = (uint16_t)rec_len;

        if (letters_count >= 3)
            letters_count = letters_count - 3;
        else
            letters_count = 0;

        uint8_t key = flip_nibbles(rec_len);

        char* temp = (char*)malloc(letters_count + 1);
        if (temp == NULL)
            break;

        uint16_t i = 0;
        while (i < letters_count)
        {
            uint8_t enc = *(data + letters_offset + i);
            *(temp + i) = (char)(enc ^ key);
            i++;
        }
        *(temp + letters_count) = '\0';

        char first = '\0';
        if (letters_count > 0)
            first = *temp;

        uint16_t mask = (uint16_t)((unsigned char)first);
        mask = (uint16_t)((mask << 8) | (unsigned char)first);

        uint16_t next_record_offset = (uint16_t)(fuzzed_next ^ mask);

        struct Word w;
        init_word(&w);
        update_word(&w, temp, letters_offset);

        free(temp);

        add_word(&list, w);

        record_offset = next_record_offset;
        count++;
    }

    return list;
}
