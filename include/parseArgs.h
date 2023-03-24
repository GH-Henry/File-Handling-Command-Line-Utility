#pragma once

/* ensure this header can be used in a C++ program */
#ifdef __cplusplus
extern "C"
{
#endif

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

#ifdef __cplusplus
    extern "C"
};
#endif