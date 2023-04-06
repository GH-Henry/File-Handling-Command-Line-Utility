#pragma once

#include "extfat.h"

void freeFileInfoStruct(fileInfo *file);
fileInfo initFileInfoStruct(char *fileName);
int verifyBoot(Main_Boot *A , Main_Boot *B);
