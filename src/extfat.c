// TODO :: Find a better way to organize functions, structs, and enums
// Need to talk to team members about this.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "extfat.h"

/* --- ENUM NOTES ---
   enumeration used to select a type of copy mode.
   To be used with function pointers so calling the
   correct function is easier and cleaner.
   pending on members to find where to put it.
*/
typedef enum _modes
{
   use_mmap  = 0,
   use_fread = 1
} modes;

/* --- STRUCT NOTES ---
   TODO :: Rename the struct type to something simpler.

   instruction and flags might be both trying to do the same thing,
   just flags has all additional flags like 'i' and 'o'.
   instructions is suppose to be the command to run like 'h', 'c', or 'v'.
   Maybe modified/fixed later on... also pending on other members to design
   a more detailed struct and where to place the definition.
*/
typedef struct _command_options
{
   char *inFile;
   char *outFile;
   char *flags;
   char instruction;
   modes mode;
} command_options_struct;

void free_command_options_struct(command_options_struct* commands)
{
   if(commands == NULL)
   {
      return;
   }

   free(commands->inFile);
   free(commands->outFile);
   free(commands->flags);
   free(commands);
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

bool get_arguments(command_options_struct *commands, int argc, char *argv[])
{
   printf("The get_arguments function ran!\n");
   printf("List of arguments :: %p %d %p\n", commands, argc, argv);
   return true;
}

bool copy_mmap(char *inputFile, char *outputFile)
{
   printf("The copy_mmap function ran!\n");
   printf("List of arguments :: %s %s\n", inputFile, outputFile);
   return true;
}

bool copy_fread(char *inputFile, char *outputFile)
{
   printf("The copy_fread function ran!\n");
   printf("List of arguments :: %s %s\n", inputFile, outputFile);
   return true;
}

bool verify_file(char *inputFile)
{
   printf("The verify_file function ran!\n");
   printf("List of arguments :: %s\n", inputFile);
   return true;
}

int main(int argc, char *argv[])
{
   if(argc == 1)
   {
      printf("No arguments... exiting\n");
      return 0;
   }

   command_options_struct *commands = (command_options_struct *)calloc(1, sizeof(command_options_struct));
   if(!get_arguments(commands, argc, argv))
   {
      printf("Error occurred... exiting\n");
      return 0;
   }
   commands->instruction = argv[1][1]; // Should be set in get_arguments function

   bool (*copy_functions[])(char *, char *) = {copy_mmap, copy_fread};

   // Preforms action based on the instruction picked up. May need to be looped.
   printf("\n");
   switch(commands->instruction)
   {
      case 'h':
         printf("Picked up 'h'... that means to provide the help manual!\n");
         print_help();
         break;
      case 'c':
         printf("Picked up 'c'... that means to copy!\n");
         copy_functions[commands->mode](commands->inFile, commands->outFile);
         break;
      case 'v':
         printf("Picked up 'v'... that means to verify!\n");
         verify_file(commands->inFile);
         break;
      default:
         printf("Unknown instruction... exiting\n");
   }

   free_command_options_struct(commands);
   commands = NULL;
   return 0;
}