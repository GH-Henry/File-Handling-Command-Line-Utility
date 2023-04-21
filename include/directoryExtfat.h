#pragma once

#include <stdint.h>

#include "extfat.h"

// Takes in the pointer to a Main_Boot struct and prints its
// corrisponding directory and files.
void printAllDirectoriesAndFiles(fileInfo *file);

int deleteFileInExfat(fileInfo *file, char *fileToDelete);