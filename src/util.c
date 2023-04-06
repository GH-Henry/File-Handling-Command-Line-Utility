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
   
   file.mainBoot = (Main_Boot *)mmap(  NULL,
                                       file.size,
                                       PROT_READ,
                                       MAP_PRIVATE,
                                       file.fd,
                                       0); // note the offset
    
   if (file.fd == -1)
   {
      perror("mmap:");
      exit(0);
   }

   int bytesPerSector = 2 << (file.mainBoot->BytesPerSectorShift - 1); // Can be added to a property of the struct

   void* fd_ptr = (void*)(intptr_t)file.fd;
   file.backupBoot = (Main_Boot *)((char*)fd_ptr + 12 * bytesPerSector);
   file.FAT = (uint32_t *)(((char*)fd_ptr) + (file.mainBoot->FatOffset * bytesPerSector));

   return file;
}

int verifyBoot(Main_Boot *A, Main_Boot *B)
{
   //BootCode usually different
   if(A->BootCode != B->BootCode)
      return 1;
   if(A->BootSignature != B->BootSignature)
      return 1;
   if(A->BytesPerSectorShift != B->BytesPerSectorShift)
      return 1;
   if(A->ClusterCount != B->ClusterCount)
      return 1;
   if(A->ClusterHeapOffset != B->ClusterHeapOffset)
      return 1;
   if(A->DriveSelect != B->DriveSelect)
      return 1;
   if(A->ExcessSpace != B->ExcessSpace)
      return 1;
   if(A->FatLength != B->FatLength)
      return 1;
   if(A->FatOffset != B->FatOffset)
      return 1;
   if(A->FileSystemName != B->FileSystemName)
      return 1;
   if(A->FileSystemRevision != B->FileSystemRevision)
      return 1;
   if(A->FirstClusterOfRootDirectory != B->FirstClusterOfRootDirectory)
      return 1;
   if(A->JumpBoot != B->JumpBoot)
      return 1;
   if(A->MustBeZero != B->MustBeZero)
      return 1;
   if(A->NumberOfFats != B->NumberOfFats)
      return 1;
   if(A->PartitionOffset != B->PartitionOffset)
      return 1;
   if(A->PercentInUse != B->PercentInUse)
      return 1;
   if(A->Reserved != B->Reserved)
      return 1;
   if(A->SectorsPerClusterShift != B->SectorsPerClusterShift)
      return 1;
   if(A->VolumeFlags != B->VolumeFlags)
      return 1;
   if(A->VolumeLength != B->VolumeLength)
      return 1;
   if(A->VolumeSerialNumber != B->VolumeSerialNumber)
      return 1;

   return 0;
}
