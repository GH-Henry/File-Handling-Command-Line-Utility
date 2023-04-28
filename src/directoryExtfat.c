#include "directoryEntryInfo.h"
#include "directoryHelperFn.h"

#define START_DIR_LVL 1

/* Deletes a target file in the exFAT image.
 * returns  0 when the file exists and is deleted 
 * returns -1 if the file is not found
 * returns  1 if the file is a directory (does not delete it) */
int deleteFileInExfat(fileInfo *file, char *fileToDelete)
{
   /* Here to help make the code look cleaner and 
    * reduce number of -> operations */
   int fd = file->fd;
   Main_Boot *MB = file->mainBoot;
   void *fp = (void *)file->mainBoot;

   // Contains information about cluster/sector size and offset location
   ClusterInfo clustInfo = {MB->ClusterHeapOffset, file->SectorSize, file->SectorsPerCluster,
                            file->SectorSize * file->SectorsPerCluster};

   // Goes to the first directory entry
   GDS_t *GDS = findCluster(MB->FirstClusterOfRootDirectory, fp, clustInfo);

   // The allocation bitmap of the exFAT file, will be used when clearing cluster data
   uint8_t *allocBitMap = findAllocBitMap(GDS, fp, clustInfo);

   // Finds the FileAndDirEntry (0x85) of the fileToDelete
   GDS_t *FileDirEntry = findFileAndDirEntry(GDS, fileToDelete, fp, clustInfo);

   // Used to check if the target file is a directory
   FileAttributes *fileAttributes = (FileAttributes *)((void *)FileDirEntry + FILE_ATTRIBUTE_OFFSET);

   /* If FileDirEntry is NULL, then findFileAndDirEntry failed to find the file (return -1)
    * If the entry is a directory, then do not delete it and return 1. */
   if (FileDirEntry == NULL)
   {
      return -1;
   }
   else if(fileAttributes->Directory)
   {
      return 1;
   }

   /* If GDS is a FileAndDirectoryEntry(0x85 EntryType), then the
    * next entry after that is the StreamExtentionEntry. */
   StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)(FileDirEntry + 1);

   /* Bit shift right by 4 is the same as integer division by 16.
    * There is always at least 1 filename entry, hence the +1.
    * One file name entry is large enough to hold 15 characters max, so if a filename
    * has 16 characters, then that means there are two filename entries. The first
    * entry contains 15 characters, the second entry contains the 16th character. */
   int numOfFilenameEntries = (streamExtEntry->NameLength >> 4) + 1;

   // Check if the file has a FirstCluster. If the value is zero, then there is no cluster or Fat chain.
   if(streamExtEntry->FirstCluster != 0)
   {
      /* Check if NoFatChain is false (equal to 0) to determine to clear the FAT chain
       * or just clear a single cluster */
      if(!streamExtEntry->NoFatChain)
      {
         clearFATChainAndData(fp, fd, file->FAT, clustInfo, streamExtEntry->FirstCluster, allocBitMap);
      }
      else // there is only one cluster of file data
      {
         clearCluster(fd, fp, streamExtEntry->FirstCluster, clustInfo, allocBitMap);
      }
   }

   /* turnOffInUseBits starts at the FileAndDirEntry, and turns off InUse bits
    * of all directory entries until it hits the last FileNameEntry. */
   turnOffInUseBits(fd, fp, FileDirEntry, numOfFilenameEntries);

   return 0;
}

/* Prints the entire directory list of the files in the exFAT image file */
void printAllDirectoriesAndFiles(fileInfo *file)
{
   Main_Boot *MB = file->mainBoot;
   void *fp = (void *)file->mainBoot;

   // Contains information about cluster/sector size and offset location
   ClusterInfo clustInfo = {MB->ClusterHeapOffset, file->SectorSize, file->SectorsPerCluster,
                            file->SectorSize * file->SectorsPerCluster};

   // Goes to the first directory entry entry
   GDS_t *GDS = findCluster(MB->FirstClusterOfRootDirectory, fp, clustInfo);

   // Prints the directory listing of the exFAT image
   printDirectory(GDS, fp, clustInfo, START_DIR_LVL);
}


char printcontent(GDS_t *GDS, void *fp, ClusterInfo clustInfo, char *Filename, char *output)
{
    int i = 0;
    char Filenameinimage[100] = "";
    char *c1ptr = NULL;
    char resultcase = 'i';
    
    

    while (GDS[i].EntryType)
    {
        // Checks if if current GDS is FileAndDirectoryEntry (0x85)
        // and the next GDS_ture (i+1) is a StreamExtensionEntry (0xc0)
        // and the one after that (i+2) is the FileNamEntry (0xc1)
         
        if (GDS[i].InUse && GDS[i].EntryType == 0x85 && GDS[i + 1].EntryType == 0xc0 && GDS[i + 2].EntryType == 0xc1)
        {
            FileAttributes *fileAttributes = (FileAttributes *)((void *)&GDS[i] + FILE_ATTRIBUTE_OFFSET);
            StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)&GDS[i + 1];
            memset(Filenameinimage,'\0', sizeof(Filenameinimage));
        
                c1ptr = (char *)&(GDS[i + 2]);
                c1ptr += 2;
                for (int j = 0; j < streamExtEntry->NameLength; j++)
                {
                    Filenameinimage[j] = *c1ptr;

                    c1ptr += 2;
                }
                 //printf("(%s)(%s)",Filename, Filenameinimage);
            
            
            if (strcmp(Filename, Filenameinimage) == 0 && fileAttributes->Directory != 1)
            {
                //printf("File found");
                resultcase = 'f';   //indicate the file is found
                FILE *fpout = fopen(output, "w");
                GDS_t *contentcluster = findCluster(streamExtEntry->FirstCluster, fp, clustInfo);
                FileNameEntry *content = NULL;
                int w = 0;

                while (contentcluster[w].EntryType)
                {
                    content = (FileNameEntry *)(void *)&contentcluster[w];
                    for (int z = 0; z < (int)sizeof(GDS_t); z++)
                    {
                     //   printf("%c", (*content).words[z]);
                        fputc((*content).FileName[z], fpout);
                    }
                    w++;
                }
                fclose(fpout);
                return resultcase;
            }
            if (strcmp(Filename, Filenameinimage) == 0 && fileAttributes->Directory == 1)
            {
                //printf("Try to open a directory\n");
                resultcase = 'd';    //indicate try to open a directory 
                return resultcase;
            }
            
            // If the attribute of the file is a directory, then recursively call this function to print its
            // contents, using its corresponding cluster, and increasing the directory level
            if (fileAttributes->Directory)
            {
                GDS_t *subGDS = findCluster(streamExtEntry->FirstCluster, fp, clustInfo);
                resultcase = printcontent(subGDS, fp, clustInfo, Filename, output);
            }
        }
        i++;
    }
    return resultcase;
}



void printfilecontent(fileInfo *file, void *fp, char *Filename, char *outputfilename)
{
    Main_Boot *MB = (Main_Boot *)fp;
    ClusterInfo clustInfo = {MB->ClusterHeapOffset, file->SectorSize, file->SectorsPerCluster,
                             file->SectorSize * file->SectorsPerCluster};
    char result;
    // directory
    GDS_t *GDS = findCluster(MB->FirstClusterOfRootDirectory, fp, clustInfo);
    // printf("1(%s)", Filename);

    result = printcontent(GDS, fp, clustInfo, Filename, outputfilename);
    switch (result)
    {
        case 'f':
        printf("File found\n");
        break;
        case 'd':
        printf("Trying to open directory, no action will be done\n");
        break;
        case 'i':
        printf("Not found\n");
        break;
        default:
        printf("Someting wrong with printcontent\n");
    }
}
