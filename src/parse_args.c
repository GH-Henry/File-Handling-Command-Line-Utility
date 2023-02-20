#include <stdio.h>

#include "parse_args.h"

void printHelp()
{
}

argument_struct_t parseArgs(int argc, char *argv[])
{
   argument_struct_t returnValue = {NULL, NULL, false, false, false};
   printf("parseArgs arguments: argc = %d, argv = %p\n", argc, argv);
   return returnValue;
}
