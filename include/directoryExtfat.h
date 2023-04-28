#pragma once

#include <stdint.h>

#include "extfat.h"

/* Prints the entire directory list of the files in the exFAT image file */
void printAllDirectoriesAndFiles(fileInfo *file);

/* Deletes a target file in the exFAT image.
 * - returns 0 when the file exists and is deleted 
 * - returns -1 if the file is not found
 * - returns 1 if the file is a directory (does not delete it) */
int deleteFileInExfat(fileInfo *file, char *fileToDelete);

void printfilecontent(void *fp, char *Filename, char *outputfilename);
