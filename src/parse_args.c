#include <stdio.h>

#include "parse_args.h"

void free_argument_struct(argument_struct_t *arguments)
{
   printf("free_argument_struct arguments: arguments = %p\n", arguments);
}

void print_help()
{
}

argument_struct_t *get_arguments(int argc, char *argv[])
{
   printf("get_arguments arguments: argc = %d, argv = %p\n", argc, argv);
   return NULL;
}
