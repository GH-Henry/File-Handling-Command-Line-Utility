#include <string.h>

#include "directoryEntryInfo.h"

#define START_DIR_LVL 1

void fetchName(char *dest, char *charPtr, int lengthOfName)
{
   for(int i = 0; i < lengthOfName; i++)
   {
      // In the event that the name is longer than 15 characters, then the next byte
      // that charPtr will point at will be 0xc1(FileName) to signal another FileNameEntry,
      // thus must offset by 2 to get to a printable character.
      if(*charPtr == (char)FileName)
      {
         charPtr += 2;
      }
      sprintf(dest, "%c", *charPtr);
      charPtr += 2; // offset by 2 to get to the next character in the name
      dest++; // move pointer to write to the next character
   }
    
   *dest = '\0'; // Writes '\0' to the end of the string
}

GDS_t *findFileAndDirEntry(GDS_t *GDS, char *fileToFind)
{
   int i = 0;
   while (GDS[i].EntryType)
   {
      // Checks if if current GDS is FileAndDirectoryEntry (FileDir - 0x85)
      // and the next GenericDirectoryStructure (i+1) is a StreamExtensionEntry (StreamExt - 0xc0)
      // and the one after that (i+2) is the FileNamEntry (FileName - 0xc1)
      if (GDS[i].InUse && GDS[i].EntryType == FileDir && GDS[i+1].EntryType == StreamExt && GDS[i+2].EntryType == FileName)
      {
         StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)&GDS[i+1];
         FileNameEntry *fileNameEntry = (FileNameEntry *)&GDS[i+2];

         char *currFilename = malloc((streamExtEntry->NameLength + 1) * sizeof(char));
         fetchName(currFilename, (char *)fileNameEntry->FileName, streamExtEntry->NameLength);

         if (strcmp(currFilename, fileToFind) == 0)
         {
            printf("Found %s\n", currFilename);
            free(currFilename);
            return &GDS[i];
         }
      }

      i++;
   }
   return NULL;
}

AllocBitmapEntry *findBitMap(GDS_t *GDS)
{
   int i = 0;
   while (GDS[i].EntryType != AllocBitMap)
   {
      i++;
   }
   return (AllocBitmapEntry *)&GDS[i];
}

void clearFATChain(uint16_t *FAT, uint8_t *bitmap, uint16_t index)
{
   uint16_t temp = 0x0000;
   do
   {
      printf("%x > ", index);
      temp = FAT[index];
      FAT[index] = 0x0000;
      index = temp;
   }
   while (index != 0xFFFF);
   printf("%x\n", index);
}

int deleteFileInExfat(fileInfo *file, char *fileToDelete)
{

   Main_Boot *MB = file->mainBoot;
   void *fp = (void *)file->mainBoot;
   uint16_t *FAT = file->FAT; // Each entry in the FAT is 16 bytes
   lseek(file->fd, (off_t)file->FAT, SEEK_SET);

   // directory
   GDS_t *GDS = FIND_CLUSTER(MB->FirstClusterOfRootDirectory, fp, MB->ClusterHeapOffset,
                             file->SectorSize, file->SectorsPerCluster);

   AllocBitmapEntry *allocBitmapEntry = findBitMap(GDS);
   uint8_t *allocBitmap = FIND_CLUSTER(allocBitmapEntry->FirstCluster, fp, MB->ClusterHeapOffset,
                                       file->SectorSize, file->SectorsPerCluster);

   GDS = findFileAndDirEntry(GDS, fileToDelete);
   if (GDS == NULL)
   {
      return -1;
   }

   // If GDS is a FileAndDirectoryEntry, then the next entry after that is the StreamExtentionEntry
   StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)(GDS + 1);
   clearFATChain(FAT, allocBitmap, streamExtEntry->FirstCluster);
   
   if (msync((void *)file->mainBoot, file->size, MS_SYNC) == -1)
   {
        perror("msync error");
        return -1;
   }

   return 0;
}


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