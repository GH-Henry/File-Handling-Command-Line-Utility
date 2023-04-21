#pragma once

#include <stdint.h>

#include "extfat.h"

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
} GDS_t;

// The Allocation Bitmap Entry struct is based on the directory entry defined here
// https://learn.microsoft.com/en-us/windows/win32/fileio/exfat-specification#71-allocation-bitmap-directory-entry
typedef struct AllocationBitmapEntry
{
    __UNION_ENTRY_TYPE__;
    union
    {
        uint8_t BitMapFlags;
        struct
        {
            // If 0 then it's the first bitmap, if 1 then it's the second bitmap
            uint8_t bitMapNum : 1;
            uint8_t reserved  : 7;
        };
    };
    uint8_t  Reserved[18];
    uint32_t FirstCluster;
    uint64_t DataLength;
} AllocBitmapEntry;

// From 7.4 File Directory Entry, the FileAttributes has an offset of 4
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#74-file-directory-entry
#define FILE_ATTRIBUTE_OFFSET 4

// The FileAttributes struct is based on the directory entry defined here
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#744-fileattributes-field
typedef struct FileAttributes
{
    uint16_t ReadOnly : 1;
    uint16_t Hidden : 1;
    uint16_t System : 1;
    uint16_t Reserved1 : 1;
    uint16_t Directory : 1;
    uint16_t Archive : 1;
    uint16_t Reserved2 : 10;
} FileAttributes;


// The StreamExtensionEntry struct is based on the directory entry defined here
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#76-stream-extension-directory-entry
typedef struct StreamExtensionEntry
{
    __UNION_ENTRY_TYPE__;
    union
    {
        uint8_t  GeneralSecondaryFlags;
        struct
        {
            uint8_t AllocationPossible : 1;
            uint8_t NoFatChain : 1;
            uint8_t Reserved : 6;
        };
    };
    uint8_t  Reserved1;
    uint8_t  NameLength;
    uint16_t NameHash;
    uint8_t  Reserved2[2];
    uint8_t  ValidDataLength[8];
    uint8_t  Reserved3[4];
    uint32_t FirstCluster;
    uint64_t DataLength;
} StreamExtensionEntry;

// The FileNameEntry struct is based on the directory entry defined here
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#77-file-name-directory-entry
typedef struct FileNameEntry
{
    __UNION_ENTRY_TYPE__;
    uint8_t GeneralSecondaryFlags;
    uint8_t FileName[30];
} FileNameEntry;