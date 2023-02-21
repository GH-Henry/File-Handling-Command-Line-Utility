#include <stdio.h>
#include <stdlib.h>

#include "parseArgs.h"
#include "copyExtfat.h"

int main(int argc, char *argv[])
{
   argument_struct_t arguments = parseArgs(argc, argv);
   if(arguments.inFile == NULL && arguments.hFlag == false)
   {
      printf("Error: missing '-i inputFile'\n");
      // Maybe use printHelp() here 
      exit(EXIT_FAILURE);
   }

   // Maybe put the if statement chain below in a function
   if(arguments.hFlag == true)
   {
      printHelp();
   }
   else if(arguments.cFlag == true)
   {
      mmapCopy(arguments.inFile, arguments.outFile);
   }
   else if(arguments.vFlag == true)
   {
      // Call the verify function (to be implemented in a later iteration)
   }
   else
   {
      printf("Unknown command...\n");
   }

   return EXIT_SUCCESS;
}