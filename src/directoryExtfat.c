#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>
#include <stdint.h>

#include "extfat.h"
#include "directoryExtfat.h"

// From 7.4 File Directory Entry, the FileAttributes have an offset of 4
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#74-file-directory-entry
#define FILE_ATTRIBUTE_OFFSET 4

typedef struct
{
    union
    {
        uint8_t EntryType;
        struct
        {
            uint8_t TypeCode : 5;
            uint8_t TypeImportance : 1;
            uint8_t TypeCategory : 1;
            uint8_t InUse : 1;
        };
    };
    uint8_t CustomDefined[19];
    uint32_t FirstCluster;
    uint64_t DataLength;
} GenericDirectoryStruct;

typedef struct
{
    uint16_t ReadOnly : 1;
    uint16_t Hidden : 1;
    uint16_t System : 1;
    uint16_t Reserved1 : 1;
    uint16_t Directory : 1;
    uint16_t Archive : 1;
    uint16_t Reserved2 : 10;
} FileAttributes;

typedef struct
{
    uint8_t  EntryType;
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

typedef struct
{
    uint8_t EntryType;
    uint8_t GeneralSecondaryFlags;
    uint8_t FileName[30];
} FileNameEntry;

// Finds cluster N
#define FIND_CLUSTER(N, fp, clustHeapOffs, bytesPerSect, sectPerCuster) \
    ((fp + clustHeapOffs * bytesPerSect) + ((N - 2) * bytesPerSect * sectPerCuster))

// Uses mmap DELETE THIS STUFF
fileInfo openImageFile()
{
    fileInfo file = {};
    // open the file system image file
    file.fd = open("test.image", O_RDWR);
    if (file.fd == -1)
    {
        perror("file open: ");
        exit(0);
    }

    struct stat statbuf;
    if (fstat(file.fd, &statbuf))
    {
        perror("stat of file:");
        exit(0);
    }
    file.size = statbuf.st_size;
    printf("The file size is %ld\n", file.size);
    
    file.M_Boot = (Main_Boot *)mmap(NULL, file.size, PROT_READ, MAP_PRIVATE, file.fd, 0);
    return file;
}
// END OF DELET THIS STUFF

void printName(char *charPtr, int legnthOfName, int dirLevel)
{
    for(int i = 0; i < dirLevel; i++)
    {
        printf("\t");
    }

    for(int i = 0; i < legnthOfName; i++)
    {
        // In the event that the name is 
        if(*charPtr == (char)0xc1)
        {
            charPtr += 2;
        }
        printf("%c", *charPtr);
        charPtr += 2;
    }

    printf("\n");
}

void printDirectory(GenericDirectoryStruct *GDS, void *fp, int clustHeapOffs, int bytesPerSector, int sectorsPerCluster, int dirLevel)
{
    int i = 0;
    while (GDS[i].EntryType)
    {
        // Checks if if current GDS is FileAndDirectoryEntry (0x85)
        // and the next GenericDirectoryStructur is a StreamExtensionEntry (0xc0)
        // and the one after that is the FileNamEntry (0xc1)
        if (GDS[i].InUse && GDS[i].EntryType == 0x85 && GDS[i+1].EntryType == 0xc0 && GDS[i+2].EntryType == 0xc1)
        {
            FileAttributes *fileAttributes = (FileAttributes *)((void *)&GDS[i] + FILE_ATTRIBUTE_OFFSET);
            StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)&GDS[i+1];
            FileNameEntry *fileNameEntry = (FileNameEntry *)&GDS[i+2];

            // If the attribute of the file is a directory, then print its name and its files.
            if (fileAttributes->Directory)
            {
                printName( (char *)fileNameEntry->FileName, streamExtEntry->NameLength, dirLevel );
                GenericDirectoryStruct *subGDS = FIND_CLUSTER(streamExtEntry->FirstCluster, fp, clustHeapOffs,
                                                              bytesPerSector, sectorsPerCluster);
                printDirectory(subGDS, fp, clustHeapOffs, bytesPerSector, sectorsPerCluster, dirLevel+1);
            }
            else // Else just print the filename
            {
                printName( (char *)fileNameEntry->FileName, streamExtEntry->NameLength, dirLevel );
            }
        }

        i++;
    }
}

void printAllDirectories()
{
    // mmap the entire file into memory
    // every data item we reference, will be relative to fp...
    fileInfo file = openImageFile();
    Main_Boot *MB = (Main_Boot *)file.M_Boot;
    void *fp = (void *)MB;

    if (fp == (void *)-1)
    {
       perror("mmap:");
       exit(0);
    }
    
    int bytesPerSector = 2 << (MB->BytesPerSectorShift - 1);
    int sectorsPerCluster = 2 << (MB->SectorsPerClusterShift - 1);

    // directory
    GenericDirectoryStruct *GDS = FIND_CLUSTER(MB->FirstClusterOfRootDirectory, fp, MB->ClusterHeapOffset,
                                               bytesPerSector, sectorsPerCluster);

    printf("The directory listing\n");
    // The directory listings are complicated.  This code just extracts the file names.
    // it does not handle directories.
    printDirectory(GDS, fp, MB->ClusterHeapOffset, bytesPerSector, sectorsPerCluster, 1);

    // unmap the file
    if (munmap(fp, file.size))
    {
        perror("error from unmap:");
        exit(0);
    }

    // close the file
    if (close(file.fd))
    {
        perror("close:");
    }
    file.fd = 0;
}