#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

#include "extfat.h"
#include "parseArgs.h"

void printHelp()
{
   printf("HELP ON HOW TO USE\nNote: -o flag must be put last\n\n");
   printf("Help:\n\"-h\"\n\n");
   printf("Copy file:\n\"-c\"\n\n");
   printf("Verify file:\n\"-v\"\nNote: Input file (-i) required\n\n");
   printf("Input file (required):\n\"-i fileName\"\n\n");
   printf("Output file:\n\"-o fileName\"\n\n");
}

argument_struct_t parseArgs(int argc, char *argv[])
{
   argument_struct_t argStruct;
   int opt;
   while((opt = getopt(argc, argv, "i:o:chv")) != -1)
   {
      switch(opt)
      {
         case 'i':
            argStruct.inFile = optarg;
            printf("DEBUG Input file is: %s\n", argStruct.inFile);
            break;
         case 'o':
            if(optarg != NULL)
               argStruct.outFile = optarg;
            else
               argStruct.outFile = argStruct.inFile;
            printf("DEBUG Output file is: %s\n", argStruct.outFile);
            break;
         case 'h':
            argStruct.hFlag = true;
            printHelp();
            break;
         case 'c':
            argStruct.cFlag = true;
            printf("C flag set to true\n");
            break;
         case 'v':
            argStruct.vFlag = true;
            printf("V flag set to true\n");
            break;
         case '?':
            if(optopt == 'o')
            {
               argStruct.outFile = argStruct.inFile;
               printf("DEBUG Output file is: %s\n", argStruct.outFile);
            }
            break;
      }
   }
   return argStruct;
}

