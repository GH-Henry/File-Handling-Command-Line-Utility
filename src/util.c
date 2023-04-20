#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "extfat.h"
#include "directoryExtfat.h"
#include "routines.h"

void freeFileInfoStruct(fileInfo *file)
{
   // unmap the file
   if (munmap(file->mainBoot, file->size))
   {
      perror("error from unmap:");
      exit(0);
   }

   // close the file
   if (close(file->fd))
   {
      perror("close");
   }
   file->fd = 0;
}

fileInfo initFileInfoStruct(char *fileName)
{
   fileInfo file = {};
   file.fd = open(fileName, O_RDWR);
   if (file.fd == -1)
   {
      perror("file open");
      exit(0);
   }

   struct stat statbuf;
   if (fstat(file.fd, &statbuf))
   {
      perror("stat of file");
      exit(0);
   }

   file.size = statbuf.st_size; // add size to the thing

   void *fp = (void *)mmap(NULL, file.size, PROT_READ, MAP_PRIVATE, file.fd, 0);

   if (fp == (void *)-1)
   {
      perror("mmap");
      exit(0);
   }

   // first, is the Main Boot record
   Main_Boot *MB = (Main_Boot *)fp;
   file.mainBoot = MB;

   int bytesPerSector = 2 << (file.mainBoot->BytesPerSectorShift - 1); // Can be added to a property of the struct
   
   void *fp_ptr = (void*)(intptr_t)fp;
   file.backupBoot = (Main_Boot *)(fp_ptr + 12 * bytesPerSector);

   file.SectorSize = bytesPerSector;
   file.SectorsPerCluster = 2 << (MB->SectorsPerClusterShift - 1);

   file.FAT = (uint32_t *)(fp_ptr + (file.mainBoot->FatOffset * bytesPerSector));
   file.fileName = fileName;

   return file;
}

int verifyBoot(fileInfo *file)
{
   uint32_t mbrChecksum = BootChecksum((uint8_t*) file->mainBoot, (short) file->SectorSize);
   uint32_t bbrChecksum = BootChecksum((uint8_t*) file->backupBoot, (short) file->SectorSize);
   
   return mbrChecksum == bbrChecksum;
}

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

GDS_t *findDirectoryEntry(GDS_t *GDS, char *fileToFind)
{
   int i = 0;
   while (GDS[i].EntryType)
   {
      // Checks if if current GDS is FileAndDirectoryEntry (FileDir - 0x85)
      // and the next GenericDirectoryStructure (i+1) is a StreamExtensionEntry (StreamExt - 0xc0)
      // and the one after that (i+2) is the FileNamEntry (FileName - 0xc1)
      if (GDS[i].InUse && GDS[i].EntryType == FileDir && GDS[i+1].EntryType == StreamExt && GDS[i+2].EntryType == FileName)
      {
         // FileAttributes *fileAttributes = (FileAttributes *)((void *)&GDS[i] + FILE_ATTRIBUTE_OFFSET);
         StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)&GDS[i+1];
         FileNameEntry *fileNameEntry = (FileNameEntry *)&GDS[i+2];

         char *currFilename = malloc((streamExtEntry->NameLength + 1) * sizeof(char));
         fetchName(currFilename, (char *)fileNameEntry->FileName, streamExtEntry->NameLength);

         if (strcmp(currFilename, fileToFind) == 0)
         {
            free(currFilename);
            return GDS;
         }
      }

      i++;
   }
   return NULL;
}