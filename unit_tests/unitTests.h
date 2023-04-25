#pragma once

#include "munit/munit.h"
#include "copyExtfat.h"
#include "directoryExtfat.h"
#include "extfat.h"
#include "parseArgs.h"
#include "routines.h"
#include "util.h"

fileInfo initFileInfoStruct(char *fileName);
MunitResult test_mmapCopy();

MunitResult test_writeByteInFile();
void writeByteInFile(char *outputFilename, size_t offset);
MunitResult test_printName();
void printName(char *charPtr, int legnthOfName, int dirLevel);
