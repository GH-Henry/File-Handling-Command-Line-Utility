#include <stdio.h>
#include <stdlib.h>

#include "extfat.h"

void free_argument_struct(argument_struct_t *arguments)
{
   if(arguments != NULL)
   {
      free(arguments->inFile);
      free(arguments->outFile);
      free(arguments->flags);
   }
   
   free(arguments);
}

/* --- FUNCTION NOTES ---
   Each function prints that they are being ran, their arguments, and returns true.
   Returning true is just to state that the function successfully ran.
   Waiting on team members to provide more details on the function prototypes and
   where exactly to put functions.
*/
bool print_help()
{
   printf("The print_help function ran!\n");
   return true;
}

bool get_arguments(argument_struct_t *arguments, int argc, char *argv[])
{
   printf("The get_arguments function ran!\n");
   printf("List of arguments :: %p %d %p\n", arguments, argc, argv);
   return true;
}

bool copy_mmap(char *input_file, char *output_file)
{
   printf("The copy_mmap function ran!\n");
   printf("List of arguments :: %s %s\n", input_file, output_file);
   return true;
}

bool copy_fread(char *input_file, char *output_file)
{
   printf("The copy_fread function ran!\n");
   printf("List of arguments :: %s %s\n", input_file, output_file);
   return true;
}

bool verify_file(char *input_file)
{
   printf("The verify_file function ran!\n");
   printf("List of arguments :: %s\n", input_file);
   return true;
}
