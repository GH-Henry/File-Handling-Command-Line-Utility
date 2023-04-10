#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "extfat.h"
#include "parseArgs.h"
#include "copyExtfat.h"
#include "directoryExtfat.h"
#include "util.h"

int main(int argc, char *argv[])
{
   argument_struct_t arguments = parseArgs(argc, argv);
   if(arguments.flags[help] == true)
   {
      printHelp();
      return EXIT_SUCCESS;
   }
   else if(arguments.inFile == NULL)
   {
      printf("Error: missing \'-i inputFile\'\n"
             "Try \'./extfat -h\' for more information\n");
      return EXIT_FAILURE;
   }

   fileInfo inputFileInfo = initFileInfoStruct(arguments.inFile);
   
   if(arguments.flags[copy] == true)
   {
      mmapCopy(arguments.inFile, arguments.outFile);
   }
   else if(arguments.flags[verify] == true)
   {
      int stat = verifyBoot(inputFileInfo.mainBoot, inputFileInfo.backupBoot);
      if(stat == 0)
         printf("Main Boot and Backup Boot are the same.\nstat = 0\n");
      else
      {
         printf("Main Boot and Backup Boot are not the same.\nstat = 1\n");
      }
   }
   else if(arguments.flags[printDirectory] == true)
   {
      printAllDirectoriesAndFiles(inputFileInfo.mainBoot);
   }
   else
   {
      printf("Unknown option\n"
             "Try \'./extfat -h\' for more information\n");
      freeFileInfoStruct(&inputFileInfo);
      return EXIT_FAILURE;
   }

   freeFileInfoStruct(&inputFileInfo);
   return EXIT_SUCCESS;
}
