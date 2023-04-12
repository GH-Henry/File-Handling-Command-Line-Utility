#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "extfat.h"
#include "copyExtfat.h"
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

   void *fp = (void *)mmap(NULL,
                           file.size,
                           PROT_READ,
                           MAP_PRIVATE,
                           file.fd,
                           0); // note the offset

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
