#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "word.h"
#include "unscramble.h"

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <command file> <output file>\n", *argv);
        return 1;
    }

    FILE* cmd = fopen(*(argv + 1), "r");
    if (cmd == NULL)
        return 1;

    FILE* out = fopen(*(argv + 2), "w");
    if (out == NULL)
    {
        fclose(cmd);
        return 1;
    }

    uint16_t num_bytes;
    char datafile[256];
    char mode[32];

    fscanf(cmd, "%hu", &num_bytes);
    fscanf(cmd, "%255s", datafile);
    fscanf(cmd, "%31s", mode);
    fclose(cmd);

    fprintf(out, "Bytes in file:  %hu\n", num_bytes);
    fprintf(out, "Name of file:   %s\n", datafile);
    fprintf(out, "Clear|Fuzzy:    %s\n", mode);

    FILE* bin = fopen(datafile, "rb");
    if (bin == NULL)
    {
        fclose(out);
        return 1;
    }

    uint8_t* data = (uint8_t*)malloc(num_bytes);
    if (data == NULL)
    {
        fclose(bin);
        fclose(out);
        return 1;
    }

    fread(data, sizeof(uint8_t), num_bytes, bin);
    fclose(bin);

    struct Word_List list;

    if (*mode == 'c' || *mode == 'C')
        list = unscramble_clear(data, num_bytes);
    else
        list = unscramble_fuzzy(data, num_bytes);

    display_words(out, list);

    free_list(list);
    free(data);
    fclose(out);

    return 0;
}
