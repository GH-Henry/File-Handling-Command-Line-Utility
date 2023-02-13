#include <stdio.h>

#include "parse_args.h"
#include "copy_extfat.h"

int main(int argc, char *argv[])
{
   argument_struct_t *arguments = get_arguments(argc, argv);
   if(arguments == NULL)
   {
      // Might explain how to use the program
      // Exit the program
   }

   // Waiting for confirmation with teammembers to
   // determine if a switch-case is appropriate.
   printf("\n");
   switch(0) // arguments->instruction is suppose to go in here
   {
      case 'h':
         print_help();
         break;
      case 'c':
         copy_mmap(arguments->inFile, arguments->outFile);
         break;
      default:
         ;
   }

   // free_argument_struct(arguments);
   // arguments = NULL;
   return 0;
}