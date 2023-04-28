#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

#include "extfat.h"
#include "parseArgs.h"
#include "copyExtfat.h"
#include "routines.h"

void printHelp()
{
   printf("HELP ON HOW TO USE\nNote: Input file (-i) required\n\n");
   printf("Help:\n\"-h\"\n\n");
   printf("Copy file:\n\"-c\"\n\n");
   printf("Verify file:\n\"-v\"\n\n");
   printf("Print directory:\n\"-d\"\n\n");
   printf("Delete a file:\n\"-D fileName\"\n\n");
   printf("Input file (required):\n\"-i fileName\"\n\n");
   printf("Output file:\n\"-o fileName\"\n\n");
}

argument_struct_t parseArgs(int argc, char *argv[])
{
   argument_struct_t argStruct = {}; // Initializes everything to zero
   int opt = 0;
   while((opt = getopt(argc, argv, "i:o::x:D:hcvd")) != -1)
   {
      switch(opt)
      {
         case 'i':
            argStruct.inFile = optarg;
            break;
         case 'o':
            argStruct.outFile = optarg;
            break;
         case 'h':
            argStruct.flags[help] = true;
            break;
         case 'c':
            argStruct.flags[copy] = true;
            break;
         case 'v':
            argStruct.flags[verify] = true;
            break;
         case 'd':
            argStruct.flags[printDir] = true;
            break;
         case 'D':
            argStruct.delFile = optarg;
            argStruct.flags[deleteFile] = true;
            break;
         case 'x':
            argStruct.flags[extractFile] = true;
            argStruct.extractFile = optarg;
            break;
         case '?':
            if(optopt == 'D')
            {
               argStruct.delFile = NULL;
               argStruct.flags[deleteFile] = true;
            }
            break;
      }
   }

   if(argStruct.outFile == NULL)
   {
      argStruct.outFile = argStruct.inFile;
   }

   return argStruct;
}
