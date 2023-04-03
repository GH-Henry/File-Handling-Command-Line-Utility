#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

#include "extfat.h"
#include "parseArgs.h"

void printHelp()
{
   printf("HELP ON HOW TO USE\nNote: Input file (-i) required\n\n");
   printf("Help:\n\"-h\"\n\n");
   printf("Copy file:\n\"-c\"\n\n");
   printf("Verify file:\n\"-v\"\n\n");
   printf("Print directory:\n\"-p\"\n\n");
   printf("Input file (required):\n\"-i fileName\"\n\n");
   printf("Output file:\n\"-o fileName\"\n\n");
}

argument_struct_t parseArgs(int argc, char *argv[])
{
   argument_struct_t argStruct = {};
   int opt = 0;
   while((opt = getopt(argc, argv, "i:o:chvp")) != -1)
   {
      switch(opt)
      {
         case 'i':
            argStruct.inFile = optarg;
            break;
         case 'o':
            if(optarg != NULL)
               argStruct.outFile = optarg;
            else
               argStruct.outFile = argStruct.inFile;
            break;
         case 'h':
            //help
            argStruct.flags[0] = true;
            break;
         case 'c':
            //copy
            argStruct.flags[1] = true;
            break;
         case 'v':
            //verify
            argStruct.flags[2] = true;
            break;
         case 'p':
            //print directory
            argStruct.flags[3] = true;
            break;
         case '?':
            if(optopt == 'o')
            {
               argStruct.outFile = argStruct.inFile;
            }
            break;
      }
   }

   if(argStruct.outFile == NULL)
      argStruct.outFile = argStruct.inFile;

   return argStruct;
}

int verifyBoot(Main_Boot *A, Main_Boot *B)
{
   if (memcmp(A, B, sizeof(Main_Boot)) == 0)
      return 0;
   else
      return 1;
}