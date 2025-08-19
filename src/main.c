#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_INPUTDEFS (10)
#define MAX_OUTPUTDEFS (10)
#define MAX_CHIPDEFS (10)

typedef struct ChipIODef
{
    char *name;
    // todo: input_array_size?
} ChipIODef;

// chip name_of_chip (inputs) -> (outputs)
typedef struct ChipDef
{
    char *name;
    ChipIODef inputs[MAX_INPUTDEFS];
    ChipIODef outputs[MAX_OUTPUTDEFS];
} ChipDef;

typedef struct TGParser
{
    char *base, *now;
    
    bool in_comment;

} TGParser;

static ChipDef g_chips[MAX_CHIPDEFS];

int main(int argc, char *argv[])
{

    return 0;
}
