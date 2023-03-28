#pragma once

#include <stdbool.h>

typedef struct
{
    char *inFile;
    char *outFile;
    bool hFlag;
    bool cFlag;
    bool vFlag;
    bool dFlag;
} argument_struct_t;

void printHelp();
argument_struct_t parseArgs(int argc, char *argv[]);
