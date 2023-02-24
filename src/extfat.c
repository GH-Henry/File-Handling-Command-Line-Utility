#include <stdio.h>
#include <stdlib.h>

#include "parseArgs.h"
#include "copyExtfat.h"

int main(int argc, char *argv[])
{
   argument_struct_t arguments = parseArgs(argc, argv);
   if(arguments.inFile == NULL && arguments.hFlag == false)
   {
      printf("Error: missing \'-i inputFile\'\n"
             "Try \'./extfat -h\' for more information\n");
      return EXIT_FAILURE;
   }

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
   else
   {
      printf("Unknown option\n"
             "Try \'./extfat -h\' for more information\n");
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}