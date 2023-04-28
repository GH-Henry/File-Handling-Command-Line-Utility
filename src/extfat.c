#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "extfat.h"
#include "parseArgs.h"
#include "copyExtfat.h"
#include "directoryExtfatUtility.h"
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
      printf("\n=== Verifying the checksums of %s ===\n", arguments.inFile);
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
      printf("\n=== Copying %s to %s ===\n", arguments.inFile, arguments.outFile);
      if( mmapCopy(&inputFileInfo, arguments.outFile) != -1 )
      {
         printf("Copied Succesfully!\n");
      }
   }

   if(arguments.flags[extractFile] == true)
   {
      if(arguments.extractFile == NULL)
      {
         printf("\n=== Missing target file to extract in %s ===\n", arguments.inFile);
      }
      else if(strcmp(arguments.outFile, arguments.inFile) == 0)
      {
         printf("\n=== Missing output file or output is same as input file ===\n");
      }
      else
      {
         // Note if file extracted doesn't exist then output file would not be created
         printf("\n=== Extracting %s from %s into %s===\n", arguments.extractFile, arguments.inFile, arguments.outFile);
         printfilecontent(inputFileInfo.mainBoot, arguments.extractFile, arguments.outFile);
      }
   }
   
   if(arguments.flags[deleteFile] == true)
   {
      if(arguments.delFile != NULL) // Checks if a target file to delete is specified
      {
         printf("\n=== Deleting %s from %s ===\n", arguments.delFile, arguments.inFile);
         switch(deleteFileInExfat(&inputFileInfo, arguments.delFile))
         {
            case -1:
               // Target file does not exist
               printf("Unable to find %s.\n", arguments.delFile);
               break;
            case  0:
               // Target file found and deleted
               printf("%s has been deleted.\n", arguments.delFile);
               break;
            case  1:
               // Target file is a directory
               printf("%s is a directory, unable to delete.\n", arguments.delFile);
               break;
         }
      }
      else
      {
         printf("\n=== Missing target file to delete in %s ===\n", arguments.inFile);
      }
   }

   if(arguments.flags[printDir] == true)
   {
      printf("\n=== Printing the directory listing of %s ===\n", arguments.inFile);
      printAllDirectoriesAndFiles(&inputFileInfo);
   }
   
   printf("\n");
   freeFileInfoStruct(&inputFileInfo);
   return EXIT_SUCCESS;
}
