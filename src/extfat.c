#include <stdio.h>
#include <stdlib.h>

#include "parseArgs.h"
#include "copyExtfat.h"
#include "extfat.h"

int main(int argc, char *argv[])
{
   /*
   Main_Boot *A = (Main_Boot*) (malloc(sizeof(Main_Boot)));
   Main_Boot *B = (Main_Boot*) (malloc(sizeof(Main_Boot)));
   */
   fileInfo bootStruct;

   argument_struct_t arguments = parseArgs(argc, argv);
   if(arguments.inFile == NULL && arguments.flags[0] == false)
   {
      printf("Error: missing \'-i inputFile\'\n"
             "Try \'./extfat -h\' for more information\n");
      return EXIT_FAILURE;
   }

   bootStruct.mainBoot = arguments.inFile;
   bootStruct.backupBoot = arguments.inFile;
   bootStruct.fileName = arguments.inFile;

   // Allows the use of different functions with indexing
   // indexing system to be added once more copy functions are defined.
   void (*copyFunctions[])(char *, char *) = {mmapCopy};

   if(arguments.flags[0] == true)
   {
      printHelp();
   }
   else if(arguments.flags[1] == true)
   {
      copyFunctions[0](arguments.inFile, arguments.outFile);
   }
   else if(arguments.flags[2] == true)
   {
      
      int stat = verifyBoot(bootStruct.mainBoot, bootStruct.backupBoot);
      if(stat == 0)
         printf("Main Boot and Backup Boot are the same.\n");
      else
      {
         printf("Main Boot and Backup Boot are not the same.\n");
         exit(EXIT_FAILURE);
      }
      
      // Call the verify function (to be implemented in a later iteration)
   }
   else if(arguments.flags[3] == true)
   {
      printf("Directory function has not been implemented yet\n");
      // Call the directory function (to be implemented in a later iteration)
   }
   else
   {
      printf("Unknown option\n"
             "Try \'./extfat -h\' for more information\n");
      return EXIT_FAILURE;
   }
   /*
   free(A);
   free(B);
   */
   return EXIT_SUCCESS;
}