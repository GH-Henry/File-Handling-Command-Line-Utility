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

/* Clears the data in the Nth cluster in an exFAT image file and turns off the
 * corresponding bit in the allocation bitmap */
void clearCluster(int fd, void *fp, int N, ClusterInfo clustInfo, uint8_t *allocBitMap);

/* Clears the FAT chain starting at FAT[index] along with the corresponding cluster data. */
void clearFATChainAndData(void *fp, int fd, FATChain *FAT, ClusterInfo clustInfo, uint32_t index, uint8_t *allocBitMap);

/* Flips the InUseBits of DirectoryEntries until it encounters the last
 * FileNameEntry. */
void turnOffInUseBits(int fd, void *fp, GDS_t *GDS, int numOfFilenameEntries);

/* Recursively traverses the exFAT image directory entries to print all files 
 * and directories out in a tabbed format. */
void printDirectory(GDS_t *GDS, void *fp, ClusterInfo clustInfo, int dirLevel);
