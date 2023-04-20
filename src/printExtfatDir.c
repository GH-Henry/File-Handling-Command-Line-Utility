#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>
#include <routines.h>

#include "util.h"
#include "directoryExtfat.h"

#define START_DIR_LVL 1

void printName(char *charPtr, int lengthOfName, int dirLevel)
{
    // Prints the proper number of tabs dependidng on dirLevel
    for(int i = 0; i < dirLevel; i++)
    {
        printf("\t");
    }

    char *filename = malloc((lengthOfName + 1) * sizeof(char));
    fetchName(filename, charPtr, lengthOfName);

    printf("%s\n", filename);
    free(filename);
}

void printDirectory(GDS_t *GDS, void *fp, int clustHeapOffs, int bytesPerSector, int sectorsPerCluster, int dirLevel)
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
                GDS_t *subGDS = FIND_CLUSTER(streamExtEntry->FirstCluster, fp, clustHeapOffs,
                                             bytesPerSector, sectorsPerCluster);
                printDirectory(subGDS, fp, clustHeapOffs, bytesPerSector, sectorsPerCluster, dirLevel+1);
            }
        }

        i++;
    }
}

void printAllDirectoriesAndFiles(fileInfo *file)
{
    Main_Boot *MB = file->mainBoot;
    void *fp = (void *)file->mainBoot;

    // directory
    GDS_t *GDS = FIND_CLUSTER(MB->FirstClusterOfRootDirectory, fp, MB->ClusterHeapOffset,
                              file->SectorSize, file->SectorsPerCluster);

    printDirectory(GDS, fp, MB->ClusterHeapOffset, file->SectorSize, file->SectorsPerCluster, START_DIR_LVL);
}