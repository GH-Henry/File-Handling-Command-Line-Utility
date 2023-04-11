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

   struct stat statbuf;
   if (fstat(file.fd, &statbuf))
   {
      perror("stat of file:");
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
      perror("mmap:");
      exit(0);
   }

   // first, is the Main Boot record
   Main_Boot *MB = (Main_Boot *)fp;
   file.mainBoot = MB;

   int bytesPerSector = 2 << (file.mainBoot->BytesPerSectorShift - 1); // Can be added to a property of the struct
   
   void *fp_ptr = (void*)(intptr_t)fp;
   file.backupBoot = (Main_Boot *)(fp_ptr + 12 * bytesPerSector);
   file.FAT = (uint32_t *)(fp_ptr + (file.mainBoot->FatOffset * bytesPerSector));
   file.fileName = fileName;

   return file;
}

int verifyBoot(Main_Boot *MB, Main_Boot *BB)
{
   if(MB->BootCode != BB->BootCode)
      return 1;
   if(MB->BootSignature != BB->BootSignature)
      return 1;
   if(MB->BytesPerSectorShift != BB->BytesPerSectorShift)
      return 1;
   if(MB->ClusterCount != BB->ClusterCount)
      return 1;
   if(MB->ClusterHeapOffset != BB->ClusterHeapOffset)
      return 1;
   if(MB->DriveSelect != BB->DriveSelect)
      return 1;
   if(MB->ExcessSpace != BB->ExcessSpace)
      return 1;
   if(MB->FatLength != BB->FatLength)
      return 1;
   if(MB->FatOffset != BB->FatOffset)
      return 1;
   if(MB->FileSystemName != BB->FileSystemName)
      return 1;
   if(MB->FileSystemRevision != BB->FileSystemRevision)
      return 1;
   if(MB->FirstClusterOfRootDirectory != BB->FirstClusterOfRootDirectory)
      return 1;
   if(MB->JumpBoot != BB->JumpBoot)
      return 1;
   if(MB->MustBeZero != BB->MustBeZero)
      return 1;
   if(MB->NumberOfFats != BB->NumberOfFats)
      return 1;
   if(MB->PartitionOffset != BB->PartitionOffset)
      return 1;
   if(MB->PercentInUse != BB->PercentInUse)
      return 1;
   if(MB->Reserved != BB->Reserved)
      return 1;
   if(MB->SectorsPerClusterShift != BB->SectorsPerClusterShift)
      return 1;
   if(MB->VolumeFlags != BB->VolumeFlags)
      return 1;
   if(MB->VolumeLength != BB->VolumeLength)
      return 1;
   if(MB->VolumeSerialNumber != BB->VolumeSerialNumber)
      return 1;

   return 0;
}
