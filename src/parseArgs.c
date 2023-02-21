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
   printf("Input file (required):\n\"-i fileName\"\n\n");
   printf("Output file:\n\"-o fileName\"\n\n");
}

argument_struct_t parseArgs(int argc, char *argv[])
{
   argument_struct_t argStruct = {NULL, NULL, false, false, false};
   int opt;
   while((opt = getopt(argc, argv, "i:o:chv")) != -1)
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
            argStruct.hFlag = true;
            break;
         case 'c':
            argStruct.cFlag = true;
            break;
         case 'v':
            argStruct.vFlag = true;
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

