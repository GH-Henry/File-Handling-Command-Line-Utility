#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "extfat.h"
#include "directoryExtfat.h"

// From 7.4 File Directory Entry, the FileAttributes have an offset of 4
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#74-file-directory-entry
#define FILE_ATTRIBUTE_OFFSET 4

// Finds cluster N
#define FIND_CLUSTER(N, fp, clustHeapOffs, bytesPerSect, sectPerCuster) \
    ((fp + clustHeapOffs * bytesPerSect) + ((N - 2) * bytesPerSect * sectPerCuster))

// The following structs are based on the directories defined here
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification
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

typedef struct
{
    char words[32];

} Actualfilecontent;

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
        // that charPtr will point at will be 0xc1 to signal another FileNameEntry,
        // thus must offset by 2 to get to a printable character.
        if(*charPtr == (char)0xc1)
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
        // Checks if if current GDS is FileAndDirectoryEntry (0x85)
        // and the next GenericDirectoryStructure (i+1) is a StreamExtensionEntry (0xc0)
        // and the one after that (i+2) is the FileNamEntry (0xc1)
        if (GDS[i].InUse && GDS[i].EntryType == 0x85 && GDS[i+1].EntryType == 0xc0 && GDS[i+2].EntryType == 0xc1)
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

    printDirectory(GDS, fp, MB->ClusterHeapOffset, bytesPerSector, sectorsPerCluster, 1);
}

char printcontent(GenericDirectoryStruct *GDS, void *fp, int clustHeapOffs, int bytesPerSector, int sectorsPerCluster, char *Filename, char *output)
{
    int i = 0;
    char Filenameinimage[100] = "";
    char *c1ptr = NULL;
    char resultcase = 'i';
    
    

    while (GDS[i].EntryType)
    {
        // Checks if if current GDS is FileAndDirectoryEntry (0x85)
        // and the next GenericDirectoryStructure (i+1) is a StreamExtensionEntry (0xc0)
        // and the one after that (i+2) is the FileNamEntry (0xc1)
         
        if (GDS[i].InUse && GDS[i].EntryType == 0x85 && GDS[i + 1].EntryType == 0xc0 && GDS[i + 2].EntryType == 0xc1)
        {
            FileAttributes *fileAttributes = (FileAttributes *)((void *)&GDS[i] + FILE_ATTRIBUTE_OFFSET);
            StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)&GDS[i + 1];
            memset(Filenameinimage,'\0', sizeof(Filenameinimage));
        
                c1ptr = (char *)&(GDS[i + 2]);
                c1ptr += 2;
                for (int j = 0; j < streamExtEntry->NameLength; j++)
                {
                    Filenameinimage[j] = *c1ptr;

                    c1ptr += 2;
                }
                 //printf("(%s)(%s)",Filename, Filenameinimage);
            
            
            if (strcmp(Filename, Filenameinimage) == 0 && fileAttributes->Directory != 1)
            {
                //printf("File found");
                resultcase = 'f';   //indicate the file is found
                FILE *fpout = fopen(output, "w");
                GenericDirectoryStruct *contentcluster = FIND_CLUSTER(streamExtEntry->FirstCluster, fp, clustHeapOffs,
                                                                      bytesPerSector, sectorsPerCluster);
                Actualfilecontent *content = NULL;
                int w = 0;

                while (contentcluster[w].EntryType)
                {
                    content = (Actualfilecontent *)(void *)&contentcluster[w];
                    for (int z = 0; z < (int)sizeof(GenericDirectoryStruct); z++)
                    {
                     //   printf("%c", (*content).words[z]);
                        fputc((*content).words[z], fpout);
                    }
                    w++;
                }
                fclose(fpout);
                return resultcase;
            }
            if (strcmp(Filename, Filenameinimage) == 0 && fileAttributes->Directory == 1)
            {
                //printf("Try to open a directory\n");
                resultcase = 'd';    //indicate try to open a directory 
                return resultcase;
            }
            
            // If the attribute of the file is a directory, then recursively call this function to print its
            // contents, using its corresponding cluster, and increasing the directory level
            if (fileAttributes->Directory)
            {
                GenericDirectoryStruct *subGDS = FIND_CLUSTER(streamExtEntry->FirstCluster, fp, clustHeapOffs,
                                                              bytesPerSector, sectorsPerCluster);
                resultcase = printcontent(subGDS, fp, clustHeapOffs, bytesPerSector, sectorsPerCluster, Filename, output);
            }
        }
        i++;
    }
    return resultcase;

}


void printfilecontent(void *fp, char *Filename, char *outputfilename)
{
    Main_Boot *MB = (Main_Boot *)fp;
    int bytesPerSector = 2 << (MB->BytesPerSectorShift - 1);
    int sectorsPerCluster = 2 << (MB->SectorsPerClusterShift - 1);
    char result;
    // directory
    GenericDirectoryStruct *GDS = FIND_CLUSTER(MB->FirstClusterOfRootDirectory, fp, MB->ClusterHeapOffset,
                                               bytesPerSector, sectorsPerCluster);
    // printf("1(%s)", Filename);

    result = printcontent(GDS, fp, MB->ClusterHeapOffset, bytesPerSector, sectorsPerCluster, Filename, outputfilename);
    switch (result)
    {
        case 'f':
        printf("File found\n");
        break;
        case 'd':
        printf("Trying to open directory, no action will be done\n");
        break;
        case 'i':
        printf("Not found\n");
        break;
        default:
        printf("Someting wrong with printcontent\n");
    }
}