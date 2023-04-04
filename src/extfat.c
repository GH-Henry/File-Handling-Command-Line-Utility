#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "parseArgs.h"
#include "copyExtfat.h"
#include "directoryExtfat.h"
#include "extfat.h"

fileInfo openFile(char *fileName)
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
   file.size = statbuf.st_size;
   file.M_Boot = mmap(NULL, file.size, PROT_READ, MAP_PRIVATE, file.fd, 0);

   return file;
}

void closeFile(fileInfo *file)
{
   // unmap the file
   if (munmap(file->M_Boot, file->size))
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

int main(int argc, char *argv[])
{
   argument_struct_t arguments = parseArgs(argc, argv);
   if(arguments.inFile == NULL && arguments.hFlag == false)
   {
      printf("Error: missing \'-i inputFile\'\n"
             "Try \'./extfat -h\' for more information\n");
      return EXIT_FAILURE;
   }

   fileInfo fileData = openFile(arguments.inFile);

   // Allows the use of different functions with indexing
   // indexing system to be added once more copy functions are defined.
   void (*copyFunctions[])(char *, char *) = {mmapCopy};
   
   if(arguments.hFlag == true)
   {
      printHelp();
   }
   else if(arguments.cFlag == true)
   {
      copyFunctions[0](arguments.inFile, arguments.outFile);
   }
   else if(arguments.vFlag == true)
   {
      printf("Verify function has not been implemented yet (Only 3 people worked on Iteration I)\n");
      // Call the verify function (to be implemented in a later iteration)
   }
   else if(arguments.dFlag == true)
   {
      printAllDirectoriesAndFiles(fileData.M_Boot);
   }
   else
   {
      printf("Unknown option\n"
             "Try \'./extfat -h\' for more information\n");
      closeFile(&fileData);
      return EXIT_FAILURE;
   }

   closeFile(&fileData);
   return EXIT_SUCCESS;
}