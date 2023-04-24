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
   GDS_t *returnVal = NULL;
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
            returnVal = &GDS[i];
         }

         free(currFilename);
      }

      i++;
   }

   return returnVal;
}

void clearFATChain(int fd, FAT_Entry *FAT, size_t offset, uint16_t index)
{
   uint16_t temp = index;

   do
   {
      lseek(fd, offset + index*sizeof(FAT_Entry), SEEK_SET);
      temp = FAT[index];
      FAT[index] = 0x0000;
      write(fd, &FAT[index], sizeof(FAT_Entry));
      index = temp;
   }
   while (index != 0xFFFF);
}

void flipInUseBits(int fd, GDS_t *GDS, size_t offset)
{
   lseek(fd, offset, SEEK_SET);
   int i = 0;
   do
   {
      GDS[i].InUse = 0;
      write(fd, &GDS[i].EntryType, 1);
      i++;
      lseek(fd, sizeof(GDS_t) - 1, SEEK_CUR); // shifts to the next directory struct
   }
   while(GDS[i].EntryType != FileDir && GDS[i].EntryType != 0x00);
}

int deleteFileInExfat(fileInfo *file, char *fileToDelete)
{
   Main_Boot *MB = file->mainBoot;
   void *fp = (void *)file->mainBoot;
   size_t offsetToFAT = (size_t)((void*)file->FAT - (void*)file->mainBoot);

   // directory
   GDS_t *GDS = FIND_CLUSTER(MB->FirstClusterOfRootDirectory, fp, MB->ClusterHeapOffset,
                             file->SectorSize, file->SectorsPerCluster);

   GDS = findFileAndDirEntry(GDS, fileToDelete);
   if (GDS == NULL)
   {
      return -1;
   }

   // If GDS is a FileAndDirectoryEntry, then the next entry after that is the StreamExtentionEntry
   StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)(GDS + 1);
   size_t offsetToFirstDirEntry = (size_t)((void *)&GDS->EntryType - (void *)file->mainBoot);

   if(!streamExtEntry->NoFatChain)
   {
      clearFATChain(file->fd, file->FAT, offsetToFAT, streamExtEntry->FirstCluster);
   }

   flipInUseBits(file->fd, GDS, offsetToFirstDirEntry);

   if (msync((void *)file->mainBoot, file->size, MS_SYNC) == -1)
   {
        perror("msync error");
        return -2;
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