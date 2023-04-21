#pragma once

#include "extfat.h"

void freeFileInfoStruct(fileInfo *file);
fileInfo initFileInfoStruct(char *fileName);
int verifyBoot(fileInfo *file);