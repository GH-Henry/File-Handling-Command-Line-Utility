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
      printf("\n=== Verifying the checksums of %s ===\n", inputFileInfo.fileName);
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
      printf("\n=== Copying %s to %s ===\n", inputFileInfo.fileName, arguments.outFile);
      if( mmapCopy(&inputFileInfo, arguments.outFile) != -1 )
      {
         printf("Copied Succesfully!\n");
      }
   }
   
   if(arguments.flags[printDirectory] == true)
   {
      printf("\n=== Printing the directory listing of %s ===\n", inputFileInfo.fileName);
      printAllDirectoriesAndFiles(inputFileInfo.mainBoot);
   }
   
   if(arguments.flags[extractDirectory] == true)
   {
      printf("\n Printing file content to outputfile (%s) Note if file extracted doesn't exist then output file would not be created\n",arguments.extractFile);
        printfilecontent(inputFileInfo.mainBoot,arguments.extractFile, arguments.outFile);
   }

   printf("\n");
   freeFileInfoStruct(&inputFileInfo);
   return EXIT_SUCCESS;
}
