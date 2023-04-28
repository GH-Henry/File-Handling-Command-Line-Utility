#pragma once
/* This file contains the function declarations of helper functions used by directoryExtfat.c. */

#include <string.h>

#include "directoryEntryInfo.h"

/* Finds the Nth Cluster in the exFAT image file */
void *findCluster(int N, void *fp, ClusterInfo c);

/* Copies the filename from FileNameEntries into dest.
 * dest should have enough space to store lengthOfName characters +1 for '\0'. */
void fetchNameFromExtFAT(char *dest, char *ptrToFilename, int lengthOfName);

/* Finds the FileAndDirEntry (Entry Type = 0x85) of a target file/directory (fileToFind) */
GDS_t *findFileAndDirEntry(GDS_t *GDS, char *fileToFind, void *fp, ClusterInfo clustInfo);

/* Finds the Allocation BitMap Location in the exFAT image file */
uint8_t *findAllocBitMap(GDS_t *GDS, void *fp, ClusterInfo clustInfo);
