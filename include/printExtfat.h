#pragma once

#include "directoryEntryInfo.h"

/* Recursively traverses the exFAT image directory entries to print all files
 * and directories out in a tabbed format. */
void printDirectory(GDS_t *GDS, void *fp, ClusterInfo *clustInfo, int dirLevel);

void printCluster(int fd, void *fp, FILE *outfile, int N, uint64_t numBytes, ClusterInfo *clustInfo);

void printAllClusterData(int fd, void *fp, FATChain *FAT, FILE *outfile,
                         StreamExt_t *streamExt, ClusterInfo *clustInfo);