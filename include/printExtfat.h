#pragma once

#include "directoryEntryInfo.h"

/* Recursively traverses the exFAT image directory entries to print all files
 * and directories out in a tabbed format. */
void printDirectory(GDS_t *GDS, void *fp, ClusterInfo clustInfo, int dirLevel);

char printcontent(GDS_t *GDS, void *fp, ClusterInfo clustInfo, char *Filename, char *output);