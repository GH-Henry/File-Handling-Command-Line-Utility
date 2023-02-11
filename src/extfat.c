// TODO :: Find a better way to organize functions, structs, and enums
// Need to talk to team members about this.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "extfat.h"

int main(int argc, char *argv[])
{
   if(argc == 1)
   {
      printf("No arguments... exiting\n");
      return 0;
   }

   // Potential need to rename the arguments variable and its type 
   // (perhaps arguments is a better name)
   argument_struct_t *arguments = (argument_struct_t *)calloc(1, sizeof(argument_struct_t));
   if(!get_arguments(arguments, argc, argv))
   {
      free_argument_struct(arguments);
      arguments = NULL;
      printf("Error occurred... exiting\n");
      return 0;
   }
   arguments->instruction = argv[1][1]; // Should be set in get_arguments function

   bool (*copy_functions[])(char *, char *) = {copy_mmap, copy_fread};

   // Preforms action based on the instruction/command picked up. May need to be looped.
   printf("\n");
   switch(arguments->instruction)
   {
      case 'h':
         printf("Picked up 'h'... that means to provide the help manual!\n");
         print_help();
         break;
      case 'c':
         printf("Picked up 'c'... that means to copy!\n");
         copy_functions[arguments->mode](arguments->inFile, arguments->outFile);
         break;
      case 'v':
         printf("Picked up 'v'... that means to verify!\n");
         verify_file(arguments->inFile);
         break;
      default:
         printf("Unknown instruction... exiting\n");
   }

   free_argument_struct(arguments);
   arguments = NULL;
   return 0;
}