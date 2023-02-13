#pragma once

/* ensure this header can be used in a C++ program */
#ifdef __cplusplus
extern "C"
{
#endif

// Waiting for task1 to give struct definition
typedef struct
{
   char *inFile;
   char *outFile;
   char *flags;
   char instruction;
} argument_struct_t;


// Waiting for task1 for function protoypes
void free_argument_struct(argument_struct_t *arguments);
argument_struct_t *get_arguments(int argc, char *argv[]);
void print_help();

#ifdef __cplusplus
    extern "C"
};
#endif