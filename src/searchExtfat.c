#include "searchExtfat.h"

/* Finds the Nth Cluster in the exFAT image file */
void *findCluster(int N, void *fp, ClusterInfo c)
{
   return ((fp + c.clustHeapOffs * c.bytesPerSect) + ((N - 2) * c.bytesPerSect * c.sectPerCluster));
}

/* Copies the filename from FileNameEntries into dest.
 * dest should have enough space to store lengthOfName characters +1 for '\0'. */
void fetchNameFromExtFAT(char *dest, char *ptrToFilename, int lengthOfName)
{
   for(int i = 0; i < lengthOfName; i++)
   {
      // In the event that the name is longer than 15 characters, then the next byte
      // that ptrToFilename will point at will be 0xc1(FileName) to signal another FileNameEntry,
      // thus must offset by 2 to get to the next printable character.
      if(*ptrToFilename == (char)FileName)
      {
         ptrToFilename += 2;
      }

      sprintf(dest, "%c", *ptrToFilename); // Prints the character into the destination string
   
      ptrToFilename += 2; // offset by 2 to get to the next character in the name
      dest++; // move pointer to write to the next character
   }
    
   *dest = '\0'; // Writes '\0' to the end of the string
}

/* Finds the FileAndDirEntry (Entry Type = 0x85) of a target file/directory (fileToFind) */
GDS_t *findFileAndDirEntry(GDS_t *GDS, char *fileToFind, void *fp, ClusterInfo clustInfo)
{
   GDS_t *returnVal = NULL; // Defaulted to NULL, so if a file is not found, NULL is returned.
   int i = 0;
   // While loop keeps running until the target file is found or until there are no more entries.
   while(GDS[i].EntryType && returnVal == NULL)
   {
      // Checks if if current GDS is FileAndDirectoryEntry (FileDir - 0x85)
      // and the next GenericDirectoryStructure (i+1) is a StreamExtensionEntry (StreamExt - 0xc0)
      // and the one after that (i+2) is the FileNamEntry (FileName - 0xc1)
      if(GDS[i].InUse && GDS[i].EntryType == FileDir && GDS[i+1].EntryType == StreamExt && GDS[i+2].EntryType == FileName)
      {
         FileAttributes *fileAttributes = (FileAttributes *)( (void *)&GDS[i] + FILE_ATTRIBUTE_OFFSET );
         StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)&GDS[i+1];
         FileNameEntry *fileNameEntry = (FileNameEntry *)&GDS[i+2];

         // Gets the filename from exFAT into a standard C string. +1 is for '\0'.
         char *currFilename = malloc((streamExtEntry->NameLength + 1) * sizeof(char));
         fetchNameFromExtFAT(currFilename, (char *)fileNameEntry->FileName, streamExtEntry->NameLength);

         /* Checks to see if the filename is equal to the target file
          * Else if a directory is encountered, recusively call this function to search through that directory
          * for the target file. */
         if(strcmp(currFilename, fileToFind) == 0)
         {
            printf("Found %s.\n", currFilename);
            returnVal = &GDS[i];
         }
         else if(fileAttributes->Directory)
         {
            GDS_t *subGDS = findCluster(streamExtEntry->FirstCluster, fp, clustInfo);
            returnVal = findFileAndDirEntry(subGDS, fileToFind, fp, clustInfo);      
         }

         free(currFilename);
      }

      i++;
   }

   return returnVal;
}

/* Finds the Allocation BitMap Location in the exFAT image file */
uint8_t *findAllocBitMap(GDS_t *GDS, void *fp, ClusterInfo clustInfo)
{
   // Searches for the AllocBitMapEntry
   int i = 0;
   while(GDS[i].EntryType != AllocBitM)
   {
      i++;
   }

   return (uint8_t *)findCluster(GDS[i].FirstCluster, fp, clustInfo);
}
