#include <stdio.h>
#include <stdlib.h>

#include "parseArgs.h"
#include "copyExtfat.h"
#include "extfat.h"
#include "util.h"

int main(int argc, char *argv[])
{
   argument_struct_t arguments = parseArgs(argc, argv);
   if(arguments.inFile == NULL && arguments.flags[0] == false)
   {
      printf("Error: missing \'-i inputFile\'\n"
             "Try \'./extfat -h\' for more information\n");
      return EXIT_FAILURE;
   }

   fileInfo A = initFileInfoStruct(arguments.inFile);

   if(arguments.flags[0] == true)
   {
      printHelp();
   }
   else if(arguments.flags[1] == true)
   {
      mmapCopy(arguments.inFile, arguments.outFile);
   }
   else if(arguments.flags[2] == true)
   {
      int stat = verifyBoot(A.mainBoot, A.backupBoot);
      if(stat == 0)
         printf("Main Boot and Backup Boot are the same.\nstat = 0\n");
      else
      {
         printf("Main Boot and Backup Boot are not the same.\nstat = 1\n");
      }
   }
   else if(arguments.flags[3] == true)
   {
      printf("Print directory function has not been implemented yet\n");
      // Call the directory function (to be implemented in a later iteration)
   }
   else
   {
      printf("Unknown option\n"
             "Try \'./extfat -h\' for more information\n");
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
