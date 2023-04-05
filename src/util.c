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
      perror("close:");
   }
   file->fd = 0;
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
   if (fstat(file.fd, &statbuf))
   {
      perror("stat of file:");
      exit(0);
   }
   file.size = statbuf.st_size; // add size to the thing
   
   file.mainBoot = (Main_Boot *)mmap(NULL,
                           size,
                           PROT_READ,
                           MAP_PRIVATE,
                           file.fd,
                           0); // note the offset

    
   if (file.fd == (void *)-1)
   {
      perror("mmap:");
      exit(0);
   }

   int bytesPerSector = 2 << (Main_Boot *)(MB.BytesPerSectorShift - 1); // Can be added to a property of the struct

   file.backupBoot = (Main_Boot *)(file.fd + 12 * bytesPerSector);

   file.FAT = (uint32_t *)((void *)file.fd + (MB->FatOffset * bytesPerSector));
   return file;
}
