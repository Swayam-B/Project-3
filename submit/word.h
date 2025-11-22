#ifndef WORD_H
#define WORD_H

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

struct Word
{
	char* word;
	uint16_t offset;
};

struct Word_List
{
	struct Word* words;
	uint16_t size;
	uint16_t count;
};

//utility functions

uint32_t strlength( const char* const word );
void strcopy( char* const destination, const char* const source );

//word functions
void init_word( struct Word *word );
void update_word( struct Word* word, const char* const wrd, uint16_t offset );
void display_word( FILE* out, struct Word word );
void free_word( struct Word word );

//word list functions
void init_list( struct Word_List *word_list, uint16_t size );
void add_word( struct Word_List *word_list, struct Word word );
struct Word get_word_at_index( struct Word_List word_list, uint16_t index );
void free_list( struct Word_List word_list );
void display_words( FILE* out, struct Word_List word_list );

#endif
