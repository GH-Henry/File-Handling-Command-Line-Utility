#pragma once

#include "extfat.h"

/* Function that maps the read memory into outputFile */
int mmapCopy(fileInfo *inputFileInfo, char *output_file);
