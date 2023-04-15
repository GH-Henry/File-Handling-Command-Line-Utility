#pragma once

#include <stdint.h>

// Finds cluster N
#define FIND_CLUSTER(N, fp, clustHeapOffs, bytesPerSect, sectPerCuster) \
    ((fp + clustHeapOffs * bytesPerSect) + ((N - 2) * bytesPerSect * sectPerCuster))

// These values are based on the Directory Entry - Entry Type subsection defined here
// http://elm-chan.org/docs/exfat_e.html
enum entryTypeCodes
{
    AllocBitMap = 0x81,
    FileDir     = 0x85,
    StreamExt   = 0xc0,
    FileName    = 0xc1
};

// The EntryType union and GenericDirectoryStruct are based on the directories defined here
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#6-directory-structure
#define __UNION_ENTRY_TYPE__        \
union                               \
{                                   \
    uint8_t EntryType;              \
    struct                          \
    {                               \
        uint8_t TypeCode : 5;       \
        uint8_t TypeImportance : 1; \
        uint8_t TypeCategory : 1;   \
        uint8_t InUse : 1;          \
    };                              \
}

typedef struct GenericDirectoryStruct
{
    __UNION_ENTRY_TYPE__;
    uint8_t  CustomDefined[19];
    uint32_t FirstCluster;
    uint64_t DataLength;
} GenericDirectoryStruct;

// The StreamExtensionEntry struct is based on the directory entry defined here
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#76-stream-extension-directory-entry
typedef struct StreamExtensionEntry
{
    __UNION_ENTRY_TYPE__;
    uint8_t  GeneralSecondaryFlags;
    uint8_t  Reserved;
    uint8_t  NameLength;
    uint8_t  NameHash[2];
    uint8_t  Reserved2[2];
    uint8_t  ValidDataLength[8];
    uint8_t  Reserved3[4];
    uint32_t FirstCluster;
    uint64_t DataLength;
} StreamExtensionEntry;

// Takes in the pointer to a Main_Boot struct and prints its
// corrisponding directory and files.
void printAllDirectoriesAndFiles(void *pointerToMB);