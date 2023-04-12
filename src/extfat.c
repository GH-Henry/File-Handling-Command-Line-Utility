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

   if(arguments.flags[verify] == true)
   {
      if(verifyBoot(&inputFileInfo) == 1)
      {
         printf("Main Boot and Backup Boot checksums are the same.\n");
      }
      else
      {
         printf("Main Boot and Backup Boot checksums are not the same.\n");
      }
   }

   if(arguments.flags[copy] == true)
   {
      if( mmapCopy(&inputFileInfo, arguments.outFile) != -1 )
      {
         printf("Copied Succesfully!\n");
      }
   }
   
   if(arguments.flags[printDirectory] == true)
   {
      printf("The directory listing\n");
      printAllDirectoriesAndFiles(inputFileInfo.mainBoot);
   }

   freeFileInfoStruct(&inputFileInfo);
   return EXIT_SUCCESS;
}
