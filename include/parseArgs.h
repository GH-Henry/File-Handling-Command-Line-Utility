#pragma once

#include <stdbool.h>
#define NUM_FLAGS 6

enum flagTypes
{
    help        = 0,
    copy        = 1,
    verify      = 2,
    printDir    = 3,
    extractFile = 4,
    deleteFile  = 5
};

typedef struct
{
    char *inFile;  // input file
    char *outFile; // output file
    char *delFile; // file to delete 
    bool flags[NUM_FLAGS];
    char *extractFile; 
} argument_struct_t;

void printHelp();
argument_struct_t parseArgs(int argc, char *argv[]);
