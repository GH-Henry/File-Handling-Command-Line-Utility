#include <stdio.h>

#include "directoryExtfat.h"



int deleteFileInExfat(fileInfo *file, char *fileToDelete)
{
    printf("%p %s\n", file, fileToDelete);
    // Main_Boot *MB = file->mainBoot;
    // void *fp = (void *)file->mainBoot;

    // directory
    //GDS_t *GDS = FIND_CLUSTER(MB->FirstClusterOfRootDirectory, fp, MB->ClusterHeapOffset,
    //                                           file->SectorSize, file->SectorsPerCluster);

    // if (findDirectoryEntry(&GDS, fileToDelete))
    // {

    // }

    
    return 0;
}