/** unscramble.c **/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "word.h"
#include "unscramble.h"

// Helper function to read a 16-bit value using pointer casting (handles little-endian)
static uint16_t read_u16(uint8_t *ptr)
{
    return *((uint16_t *)ptr);
}

// Helper function to flip the nybbles of a byte
static uint8_t flip_nibbles(uint8_t x)
{
    // (High nibble >> 4) | (Low nibble << 4)
    return (uint8_t)((x >> 4) | (x << 4));
}

// --- CLEAR UNSCRAMBLING ---
struct Word_List unscramble_clear(uint8_t *data, uint16_t num_bytes)
{
    (void)num_bytes;

    uint16_t num_words = read_u16(data);
    uint16_t record_offset = read_u16(data + 2); // Initial offset is clear

    struct Word_List list;
    init_list(&list, num_words);

    uint16_t count = 0;

    while (count < num_words)
    {
        uint8_t rec_len = *(data + record_offset);
        uint16_t next_record_offset = read_u16(data + record_offset + 1);

        uint16_t letters_offset = record_offset + 3;
        uint16_t letters_count = rec_len >= 3 ? rec_len - 3 : 0;

        char *temp = malloc(letters_count + 1);
        if (!temp) break;

        uint16_t i = 0;
        while (i < letters_count)
        {
            // Use pointer arithmetic (no array notation)
            *(temp + i) = (char)*(data + letters_offset + i);
            i++;
        }
        *(temp + letters_count) = '\0';

        struct Word w;
        init_word(&w);
        // Use the start of the record as the offset, based on typical output
        update_word(&w, temp, record_offset); 

        free(temp);
        add_word(&list, w);

        record_offset = next_record_offset;
        count++;
    }

    return list;
}

// --- FUZZY UNSCRAMBLING ---
struct Word_List unscramble_fuzzy(uint8_t *data, uint16_t num_bytes)
{
    (void)num_bytes;

    uint16_t num_words = read_u16(data);
    // FIX: The initial record offset (data + 2) is CLEAR (unfuzzed).
    uint16_t record_offset = read_u16(data + 2); 

    struct Word_List list;
    init_list(&list, num_words);

    uint16_t count = 0;

    while (count < num_words)
    {
        uint8_t rec_len = *(data + record_offset);
        
        // This is the fuzzed offset to the next word record
        uint16_t fuzzed_next = read_u16(data + record_offset + 1);

        uint16_t letters_offset = record_offset + 3;
        uint16_t letters_count = rec_len >= 3 ? rec_len - 3 : 0;

        // --- 1. Unfuzz the Word Characters (XOR with flipped record length) ---
        uint8_t key_len = flip_nibbles(rec_len);

        char *temp = malloc(letters_count + 1);
        if (!temp) break;

        uint16_t i = 0;
        while (i < letters_count)
        {
            uint8_t enc = *(data + letters_offset + i);
            *(temp + i) = (char)(enc ^ key_len);
            i++;
        }
        *(temp + letters_count) = '\0';

        // --- 2. Unfuzz the Next Offset (XOR with first letter mask) ---
        // Get the decoded first letter of the *current* word
        unsigned char first_letter = (letters_count > 0 ? *(temp + 0) : 0);

        // Create the 16-bit mask: C C
        uint16_t mask = ((uint16_t)first_letter << 8) | (uint16_t)first_letter;

        // Apply XOR to get the offset for the *next* word record
        uint16_t next_record_offset = (uint16_t)(fuzzed_next ^ mask);

        // Build and add the word
        struct Word w;
        init_word(&w);
        // Use the start of the record as the offset
        update_word(&w, temp, record_offset); 
        add_word(&list, w);

        free(temp);

        // Update to the newly unfuzzed offset for the next loop iteration
        record_offset = next_record_offset;
        count++;
    }

    return list;
}