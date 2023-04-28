#include "searchExtfat.h"
#include "printExtfat.h"

/* ===== Additional internal helper functions/Macro ===== */

#define MIN(x, y) (x <= y) ? x : y

/* Prints the filename inside the exFAT image with the proper amount of tabs
 * depending on the dirLevel. */
void printFileName(char *ptrToFilename, int lengthOfName, int dirLevel)
{
    // Prints the proper number of tabs dependidng on dirLevel
    for (int i = 0; i < dirLevel; i++)
    {
        printf("\t");
    }

    // Gets the filename from exFAT into a standard C string. +1 is for '\0'.
    char *filename = malloc((lengthOfName + 1) * sizeof(char));
    fetchNameFromExtFAT(filename, ptrToFilename, lengthOfName);

    printf("%s\n", filename);
    free(filename);
}

/* ===== End of Additional internal helper functions ===== */


/* ===== Functions declared in printExtfat.h ===== */

/* Recursively traverses the exFAT image directory entries to print all files
 * and directories out in a tabbed format. Uses printFileName() to print the names. */
void printDirectory(GDS_t *GDS, void *fp, ClusterInfo *clustInfo, int dirLevel)
{
    int i = 0;
    while (GDS[i].EntryType)
    {
        // Checks if if current GDS is FileAndDirectoryEntry (FileDir - 0x85)
        // and the next GenericDirectoryStructure (i+1) is a StreamExtEntry (StreamExt - 0xc0)
        // and the one after that (i+2) is the FileNamEntry (FileName - 0xc1)
        if (GDS[i].InUse && GDS[i].EntryType == FileDir && GDS[i + 1].EntryType == StreamExt && GDS[i + 2].EntryType == FileName)
        {
            FileAttributes *fileAttributes = (FileAttributes *)((void *)&GDS[i] + FILE_ATTRIBUTE_OFFSET);
            StreamExt_t *streamExtEntry = (StreamExt_t *)&GDS[i + 1];
            FileNameEntry *fileNameEntry = (FileNameEntry *)&GDS[i + 2];

            printFileName((char *)fileNameEntry->FileName, streamExtEntry->NameLength, dirLevel);

            // If the attribute of the file is a directory, then recursively call this function to print its
            // contents, using its corresponding cluster, and increasing the directory level
            if (fileAttributes->Directory)
            {
                GDS_t *subGDS = findCluster(streamExtEntry->FirstCluster, fp, clustInfo);
                printDirectory(subGDS, fp, clustInfo, dirLevel + 1);
            }
        }

        i++;
    }
}

void printCluster(int fd, void *fp, FILE *outfile, int N, uint64_t numBytes, ClusterInfo *clustInfo)
{
    void *cluster = findCluster(N, fp, clustInfo);

    lseek(fd, (off_t)(cluster - fp), SEEK_SET);

    fwrite(cluster, 1, numBytes, outfile);
}

void printAllClusterData(int fd, void *fp, FATChain *FAT, FILE *outfile, StreamExt_t *streamExt, ClusterInfo *clustInfo)
{
    uint32_t index = streamExt->FirstCluster;

    uint64_t numBytes = MIN(streamExt->ValidDataLength, streamExt->DataLength);
    do
    {
        if(numBytes > clustInfo->bytesPerCluster)
        {
            printCluster(fd, fp, outfile, index, clustInfo->bytesPerCluster, clustInfo);
            numBytes -= clustInfo->bytesPerCluster;
        }
        else
        {
            printCluster(fd, fp, outfile, index, numBytes, clustInfo);
            numBytes = 0;
        }

        index = FAT[index]; // sets up for the next FAT entry in the chain
    } while (index != 0xFFFFFFFF);
}

/* ===== End of Functions declared in printExtfat.h ===== */