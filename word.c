/* As a Hokie, I will conduct myself with honor and integrity at
all times. I will not lie, cheat, or steal, nor will I accept the
actions of those who do. */

#include "word.h"

uint32_t strlength(const char* const word)
{
    uint32_t len = 0;

    if (word == NULL)
        return 0;

    while (*(word + len) != '\0')
        len++;

    return len;
}

// Copies source string into destination string
void strcopy(char* const destination, const char* const source)
{
    uint32_t i = 0;

    if (destination == NULL || source == NULL)
        return;

    while (*(source + i) != '\0')
    {
        *(destination + i) = *(source + i);
        i++;
    }

    *(destination + i) = '\0';
}

// Initializes a Word structure
void init_word(struct Word* word)
{
    word->word   = NULL;
    word->offset = 0;
}

// Updates a Word structure with new values
void update_word(struct Word* word, const char* const wrd, uint16_t offset)
{
    if (word->word != NULL)
    {
        free(word->word);
        word->word = NULL;
    }

    uint32_t len = strlength(wrd);

    char* temp = (char*)calloc(len + 1, sizeof(char));
    assert(temp != NULL);

    strcopy(temp, wrd);

    word->word   = temp;
    word->offset = offset;
}

// Displays a Word structure to the given file
void display_word(FILE* out, struct Word word)
{
    const char* to_print = word.word;

    if (to_print == NULL)
        to_print = "";

    fprintf(out, "%-20s @ Offset: %02x\n", to_print, word.offset);
}

// Frees memory allocated for a Word structure
void free_word(struct Word word)
{
    if (word.word != NULL)
        free(word.word);
}

// Initializes a Word_List structure with a given size
void init_list(struct Word_List* word_list, uint16_t size)
{
    word_list->words = (struct Word*)calloc(size, sizeof(struct Word));
    assert(word_list->words != NULL);

    word_list->size  = size;
    word_list->count = 0;

    for (uint16_t i = 0; i < size; i++)
        init_word(word_list->words + i);
}

// Adds a Word to the Word_List, resizing if necessary
void add_word(struct Word_List* word_list, struct Word word)
{
    if (word_list->count == word_list->size)
    {
        uint16_t new_size = word_list->size * 2;

        struct Word* new_array =
            (struct Word*)realloc(word_list->words,
                                  new_size * sizeof(struct Word));
        assert(new_array != NULL);

        word_list->words = new_array;

        for (uint16_t i = word_list->size; i < new_size; i++)
            init_word(word_list->words + i);

        word_list->size = new_size;
    }

    *(word_list->words + word_list->count) = word;
    word_list->count++;
}

// Gets a copy of the Word at the given index
struct Word get_word_at_index(struct Word_List word_list, uint16_t index)
{
    struct Word result;
    init_word(&result);

    if (index >= word_list.count)
    {
        update_word(&result, "Word Not Found", 0);
        return result;
    }

    struct Word* original = word_list.words + index;

    if (original->word != NULL)
        update_word(&result, original->word, original->offset);
    else
        result.offset = original->offset;

    return result;
}

// Frees memory allocated for a Word_List structure
void free_list(struct Word_List word_list)
{
    for (uint16_t i = 0; i < word_list.count; i++)
        free_word(*(word_list.words + i));

    if (word_list.words != NULL)
        free(word_list.words);
}

// Displays all words in the Word_List to the given file
void display_words(FILE* out, struct Word_List word_list)
{
    for (uint16_t i = 0; i < word_list.count; i++)
        display_word(out, *(word_list.words + i));
}
