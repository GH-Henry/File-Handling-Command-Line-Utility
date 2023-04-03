#pragma once

#include <stdbool.h>
#define NUM_FLAGS 4

enum flagTypes
{
    help = 0,
    copy = 1,
    verify = 2,
    printDirectory = 3
};

typedef struct
{
    char *inFile;
    char *outFile;
    bool flags[4];
} argument_struct_t;

void printHelp();
argument_struct_t parseArgs(int argc, char *argv[]);
