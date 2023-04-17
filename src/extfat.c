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

// DEBUG INFO... To Delete
   // printf("%s %s %s\n", arguments.inFile, arguments.outFile, arguments.delFile);
   // for(int i = 0; i < NUM_FLAGS; i++)
   // {
   //    printf("%d ", arguments.flags[i]);
   // }
   // printf("\n");

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

   if(arguments.flags[deleteFile] == true)
   {
      if(arguments.delFile != NULL)
      {
         printf("\n=== Deleting %s from %s ===\n", arguments.delFile, inputFileInfo.fileName);
      }
      else
      {
         printf("\n=== Missing target file to delete in %s ===\n", inputFileInfo.fileName);
      }
   }

   if(arguments.flags[copy] == true && arguments.outFile != NULL)
   {
      printf("\n=== Copying %s to %s ===\n", inputFileInfo.fileName, arguments.outFile);
      if( mmapCopy(&inputFileInfo, arguments.outFile) != -1 )
      {
         printf("Copied Succesfully!\n");
      }
   }
   
   if(arguments.flags[printDir] == true)
   {
      printf("\n=== Printing the directory listing of %s ===\n", inputFileInfo.fileName);
      printAllDirectoriesAndFiles(inputFileInfo.mainBoot);
   }

   printf("\n");
   freeFileInfoStruct(&inputFileInfo);
   return EXIT_SUCCESS;
}
