#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>

#include "extfat.h"
#include "directoryExtfat.h"

#define START_DIR_LVL 1

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

// The FileNameEntry struct is based on the directory entry defined here
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#77-file-name-directory-entry
typedef struct FileNameEntry
{
    uint8_t EntryType;
    uint8_t GeneralSecondaryFlags;
    uint8_t FileName[30];
} FileNameEntry;

void printName(char *charPtr, int legnthOfName, int dirLevel)
{
    // Prints the proper number of tabs dependidng on dirLevel
    for(int i = 0; i < dirLevel; i++)
    {
        printf("\t");
    }

    for(int i = 0; i < legnthOfName; i++)
    {
        // In the event that the name is longer than 15 characters, then the next byte
        // that charPtr will point at will be 0xc1(FileName) to signal another FileNameEntry,
        // thus must offset by 2 to get to a printable character.
        if(*charPtr == (char)FileName)
        {
            charPtr += 2;
        }
        printf("%c", *charPtr);
        charPtr += 2; // offset by 2 to get to the next character in the name
    }

    printf("\n");
}

void printDirectory(GenericDirectoryStruct *GDS, void *fp, int clustHeapOffs, int bytesPerSector, int sectorsPerCluster, int dirLevel)
{
    int i = 0;
    while (GDS[i].EntryType)
    {
        // Checks if if current GDS is FileAndDirectoryEntry (FileDir - 0x85)
        // and the next GenericDirectoryStructure (i+1) is a StreamExtensionEntry (StreamExt - 0xc0)
        // and the one after that (i+2) is the FileNamEntry (FileName - 0xc1)
        if (GDS[i].InUse && GDS[i].EntryType == FileDir && GDS[i+1].EntryType == StreamExt && GDS[i+2].EntryType == FileName)
        {
            FileAttributes *fileAttributes = (FileAttributes *)((void *)&GDS[i] + FILE_ATTRIBUTE_OFFSET);
            StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)&GDS[i+1];
            FileNameEntry *fileNameEntry = (FileNameEntry *)&GDS[i+2];

            printName( (char *)fileNameEntry->FileName, streamExtEntry->NameLength, dirLevel );

            // If the attribute of the file is a directory, then recursively call this function to print its
            // contents, using its corresponding cluster, and increasing the directory level
            if (fileAttributes->Directory)
            {
                GenericDirectoryStruct *subGDS = FIND_CLUSTER(streamExtEntry->FirstCluster, fp, clustHeapOffs,
                                                              bytesPerSector, sectorsPerCluster);
                printDirectory(subGDS, fp, clustHeapOffs, bytesPerSector, sectorsPerCluster, dirLevel+1);
            }
        }

        i++;
    }
}

void printAllDirectoriesAndFiles(void *fp)
{
    Main_Boot *MB = (Main_Boot *)fp;
    int bytesPerSector = 2 << (MB->BytesPerSectorShift - 1);
    int sectorsPerCluster = 2 << (MB->SectorsPerClusterShift - 1);

    // directory
    GenericDirectoryStruct *GDS = FIND_CLUSTER(MB->FirstClusterOfRootDirectory, fp, MB->ClusterHeapOffset,
                                               bytesPerSector, sectorsPerCluster);

    printDirectory(GDS, fp, MB->ClusterHeapOffset, bytesPerSector, sectorsPerCluster, START_DIR_LVL);
}