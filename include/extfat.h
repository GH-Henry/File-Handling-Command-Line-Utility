#pragma once

/* ensure this header can be used in a C++ program */
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

// This header defines the layout of data on an extfat disk image.
// For the details, please refer to
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification

typedef struct
{
    char JumpBoot[3];
    char FileSystemName[8];
    char MustBeZero[53];
    long int PartitionOffset;
    long int VolumeLength;
    int FatOffset;
    int FatLength;
    int ClusterHeapOffset;
    int ClusterCount;
    int FirstClusterOfRootDirectory;
    int VolumeSerialNumber;
    short int FileSystemRevision;
    short int VolumeFlags;
    unsigned char BytesPerSectorShift;
    unsigned char SectorsPerClusterShift;
    unsigned char NumberOfFats;
    unsigned char DriveSelect;
    unsigned char PercentInUse;
    unsigned char Reserved[7];
    unsigned char BootCode[390];
    short int BootSignature;
    unsigned char ExcessSpace;
} Main_Boot;

/* --- ENUM NOTES ---
   enumerations used to select a type of copy mode.
   To be used with an array of functions so calling the
   correct function is easier and cleaner.
   Pending on members to find out where to put it.
*/
typedef enum
{
   use_mmap  = 0,
   use_fread = 1
} modes;

/* --- STRUCT NOTES ---
   ! Rename struct type

   instruction and flags might be both trying to do the same thing,
   just flags has all additional flags like 'i' and 'o'.
   instructions is suppose to be the command to run like 'h', 'c', or 'v'.
   May be modified/fixed later on... also pending task1 member for the
   design of the struct and where to place the definition.
*/
typedef struct
{
   char *inFile;
   char *outFile;
   char *flags;
   char instruction;
   modes mode;
} argument_struct_t;

// Function definitions are located in ./src/utils.c for now. Ask teammates for
// what they have in mind or where the functions should be located.
void free_argument_struct(argument_struct_t *arguments);
bool print_help();
bool get_arguments(argument_struct_t *arguments, int argc, char *argv[]);
bool copy_mmap(char *input_file, char *output_file);
bool copy_fread(char *input_file, char *output_file);
bool verify_file(char *input_file);

#ifdef __cplusplus
    extern "C"
};
#endif
