#include <stdio.h>
#include <stdlib.h>

#include "util.h"

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
      perror("close:");
   }
   fd = 0;
}

fileInfo initFileInfoStruct(char *fileName)
{
   fileInfo file = {};
   file.fd = open(fileName, O_RDWR);
   if (file.fd == -1)
   {
      perror("file open: ");
      exit(0);
   }
   off_t size = 0;
   struct stat statbuf;
   if (fstat(fd, &statbuf))
   {
      perror("stat of file:");
      exit(0);
   }
   file.size = statbuf.st_size; // add size to the thing
   
   file.mainBoot = = (Main_Boot *)mmap(NULL,
                           size,
                           PROT_READ,
                           MAP_PRIVATE,
                           fd,
                           0); // note the offset

    
   if (fp == (void *)-1)
   {
      perror("mmap:");
      exit(0);
   }

   int bytesPerSector = 2 << (MB->BytesPerSectorShift - 1); // Can be added to a property of the struct

   file.backupBoot = (Main_Boot *)(fp + 12 * bytesPerSector);

   file.FAT = (uint32_t *)((void *)fp + (MB->FatOffset * bytesPerSector));
   return file;
}
