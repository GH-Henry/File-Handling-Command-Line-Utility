/*
   This is an example of using mmap to read an extFAT image file.
   To make the sample file:
    % # create the file system image
    % dd if=/dev/zero of=test.image count=1 bs=1G
    % sudo losetup /dev/loop2 test.image
    % sudo /usr/sbin/mkexfatfs /dev/loop2
    % # put something in the file system image
    % mkdir /tmp/d
    % sudo mount /dev/loop2 /tmp/d
    % cp examples/mmap.c /tmp/d
    % # clean up
    % sudo umount /tmp/d
    % sudo losetup -d /dev/loop2
    % rm -rf /tmp/d
    % rm test.image
   Written by Bud Davis, jimmie.davis@uta.edu
   (c) 2023, All Rights Reserved
   Provided to students of CSE3310, UTA. Any use
   other than this course is prohibited.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

#include "extfat.h"
#include "task1.h"

int main(int argc, char *argv[])
{
   argument_struct_t yeet = parseArgs(argc, argv);

   // open the file system image file
   int fd = open(yeet.inFile, O_RDWR);
   if (fd == -1)
   {
      perror("file open: ");
      exit(0);
   }

   // Take the pointer returned from mmap() and turn it into
   // a structure that understands the layout of the data
   Main_Boot *MB = (Main_Boot *)mmap(NULL,
                                     sizeof(Main_Boot),
                                     PROT_READ,
                                     MAP_PRIVATE,
                                     fd,
                                     0); // note the offset

   if (MB == (Main_Boot *)-1)
   {
      perror("error from mmap:");
      exit(0);
   }

   // print out some things we care about

   printf("the pointer to MB is %p  \n", MB);

   printf("JumpBoot  %d %d %d \n", MB->JumpBoot[0], MB->JumpBoot[1], MB->JumpBoot[2]);
   printf("FileSystemName %s\n", MB->FileSystemName); // warning, not required to be terminated
   printf("PartitionOffset %ld\n", MB->PartitionOffset);
   printf("VolumeLength %ld\n", MB->VolumeLength);
   printf("FatOffset %d\n", MB->FatOffset);
   printf("FatLength %d\n", MB->FatLength);
   printf("ClusterHeapOffset %d\n", MB->ClusterHeapOffset);
   printf("ClusterCount %d\n", MB->ClusterCount);
   printf("FirstClusterofRootDirectory %d\n", MB->FirstClusterOfRootDirectory);
   printf("VolumeSerialNumber %x\n", MB->VolumeSerialNumber);
   printf("PercentInUse %d\n", MB->PercentInUse);
   

   // check that the offsets match the documentation
   assert(offsetof(Main_Boot, PercentInUse) == 112);
   //printf("the offset of PerCentInUse %ld\n", offsetof(Main_Boot, PercentInUse));

   printf("BytesPerSectorShift %d\n",MB->BytesPerSectorShift);
   printf("SectorsPerClusterShift %d\n",MB->SectorsPerClusterShift);
   printf("NumberOfFats %d\n",MB->NumberOfFats);

   int bytesPerSector = 2  << (MB->BytesPerSectorShift - 1 );
   int sectorsPerCluster = 2 << (MB->SectorsPerClusterShift - 1 );

   printf("-----> the bytesPerSector are %d\n",bytesPerSector);
   printf("-----> the sectorsPerCluster are %d\n",sectorsPerCluster);

   
   // unmap the file
   if (munmap(MB, sizeof(Main_Boot)) == -1)
   {
      perror("error from unmap:");
      exit(0);
   }

   // close the file
   if (close(fd))
   {
      perror("closeStat:");
   }
   fd = 0;

/*
   if(argStruct.inFile != NULL)
   {
      free(argStruct.inFile);
   }
   if(argStruct.outFile != NULL)
   {
      free(argStruct.outFile);
   }
*/
   //free(argStruct);

   return 0;
}

void printHelp()
{
   printf("HELP ON HOW TO USE\nNote: -o flag must be put last\n\n");
   printf("Help:\n\"-h\"\n\n");
   printf("Copy file:\n\"-c\"\n\n");
   printf("Verify file:\n\"-v\"\nNote: Input file (-i) required\n\n");
   printf("Input file (required):\n\"-i fileName\"\n\n");
   printf("Output file:\n\"-o fileName\"\n\n");
}

argument_struct_t parseArgs(int argc, char *argv[])
{
   argument_struct_t argStruct;
   int opt;
   while((opt = getopt(argc, argv, "i:o:chv")) != -1)
   {
      switch(opt)
      {
         case 'i':
            argStruct.inFile = optarg;
            printf("DEBUG Input file is: %s\n", argStruct.inFile);
            break;
         case 'o':
            if(optarg != NULL)
               argStruct.outFile = optarg;
            else
               argStruct.outFile = argStruct.inFile;
            printf("DEBUG Output file is: %s\n", argStruct.outFile);
            break;
         case 'h':
            argStruct.hFlag = true;
            printHelp();
            break;
         case 'c':
            argStruct.cFlag = true;
            printf("C flag set to true\n");
            break;
         case 'v':
            argStruct.vFlag = true;
            printf("V flag set to true\n");
            break;
         case '?':
            if(optopt == 'o')
            {
               argStruct.outFile = argStruct.inFile;
               printf("DEBUG Output file is: %s\n", argStruct.outFile);
            }
            break;
      }
   }
   return argStruct;
}
/*
void copyFiles(char *fileName1, char fileName2)
{
   int file1 = open(fileName1, O_RDWR);
   if (file1 == -1)
   {
      perror("file open: ");
      exit(0);
   }

   int file2 = open(fileName2, O_RDWR);
   if (file2 == -1)
   {
      perror("file open: ");
      exit(0);
   }
}
*/
