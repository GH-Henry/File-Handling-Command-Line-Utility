#include <string.h>

#include "directoryEntryInfo.h"

#define START_DIR_LVL 1

/* === Utility Functions to Interact with the exFAT === */

/* Finds the Nth Cluster in the exFAT image file */
void *findCluster(int N, void *fp, ClusterInfo c)
{
   return ((fp + c.clustHeapOffs * c.bytesPerSect) + ((N - 2) * c.bytesPerSect * c.sectPerCluster));
}

/* Copies the filename from FileNameEntries into dest.
 * dest should have enough space to store lengthOfName characters. */
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
            printf("Found %s\n", currFilename);
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

/* clears the data in the Nth cluster in an exFAT image file */
void clearCluster(int fd, void *fp, int N, ClusterInfo clustInfo)
{
   void *cluster = findCluster(N, fp, clustInfo); // Gets cluster N

   uint8_t zero = 0; // Byte of data to write
   lseek(fd, (size_t)(cluster - fp), SEEK_SET); // Seeks to cluster N in the image file

   // Clear the entire cluster to zeros
   for(size_t i = 0; i < clustInfo.bytesPerCluster; i++)
   {
      write(fd, &zero, 1);
   }
}

/* clears the FAT chain starting at FAT[index] along with clearing the cluster data. */
void clearFATChainAndData(void *fp, int fd, FATChain *FAT, ClusterInfo clustInfo, uint32_t index)
{
   // Number of bytes from the start of the file to the FAT.
   size_t offset = (size_t)((void *)FAT - fp);

   uint32_t temp = index; // Used to remember the next FAT location to got to

   do
   {
      lseek(fd, offset + index*sizeof(FATChain), SEEK_SET); // Seeks to FAT[index]
      temp = FAT[index]; // Stores the current value
      FAT[index] = 0x00000000; // Wipes the FAT[index]
      write(fd, &FAT[index], sizeof(FATChain)); // Writes wiped value into the image file
      clearCluster(fd, fp, index, clustInfo); // Clears the corresponding cluster
      index = temp; // sets up for the next FAT entry in the chain
   }
   while (index != 0xFFFFFFFF);
}

/* Turns the InUse bit off of FileNameEntries and empties the data. */
void clearFileNameData(int fd, void *fp, FileNameEntry *firstEntry, int numOfFilenameEntries)
{
   // An empty FileNameEntry used to copy over into the image file to
   // empty out the name of the file.
   FileNameEntry emptyFileNameEntry = {};

   // FileName is 0xc1, the value is part of an enum defined in directoryEntryInfo.h
   emptyFileNameEntry.EntryType = FileName;
   emptyFileNameEntry.InUse = 0; // Turns off the InUse bit

   // Move to the target FileNameEntry in the image
   lseek(fd, (off_t)((void *)firstEntry - fp), SEEK_SET);
 
   // Goes through each FileNameEntry belonging to the file
   for(int i = 0; i < numOfFilenameEntries; i++)
   {
      // Writes the emptyFileNameEntry to the exFAT image file
      write(fd, &emptyFileNameEntry, sizeof(FileNameEntry));
   }
}

/* Flips the InUseBits of DirectoryEntries until it encounters the last
 * FileNameEntry. Uses clearFileNameData() to empty and turn off the InUse bit
 * of FileNameEntries. */
void turnOffInUseBits(int fd, void *fp, GDS_t *GDS, int numOfFilenameEntries)
{
   // Seek to the directory struct in the file
   lseek(fd, (off_t)((void *)GDS - fp), SEEK_SET);

   /* Note that the first directory should be a FileAndDirEntry (0x85), then
    * the one after that is StreamExtEntry (0xc0), which the one following that
    * is FileNameEntry (0xc1). When FileNameEntry is encountered, leave the 
    * loop and use clearFileNameData() to handle the last entries corresponding
    * to the file to delete. */
   int i = 0;
   do
   {
      GDS[i].InUse = 0; // Set the InUse bit to off
      write(fd, &GDS[i].EntryType, 1); // Write it into the file
      i++; // Move to the next directory struct

      /* shifts to the next directory struct in the file, -1 is 
       * there to account for the offset from write() writing a byte */
      lseek(fd, sizeof(GDS_t) - 1, SEEK_CUR);
   }
   while(GDS[i].EntryType != FileName);

   // Clears and turns off InUse bits of the FileNameEntries
   clearFileNameData(fd, fp, (FileNameEntry *)&GDS[i], numOfFilenameEntries);
}

/* Prints the filename inside the exFAT image with the proper amount of tabs
 * depending on the dirLevel. */
void printFileName(char *ptrToFilename, int lengthOfName, int dirLevel)
{
   // Prints the proper number of tabs dependidng on dirLevel
   for(int i = 0; i < dirLevel; i++)
   {
      printf("\t");
   }

   // Gets the filename from exFAT into a standard C string. +1 is for '\0'.
   char *filename = malloc((lengthOfName + 1) * sizeof(char));
   fetchNameFromExtFAT(filename, ptrToFilename, lengthOfName);

   printf("%s\n", filename);
   free(filename);
}

/* Recursively traveres the exFAT image directory entries to print all files 
 * and directories out in a tabbed format. */
void printDirectory(GDS_t *GDS, void *fp, ClusterInfo clustInfo, int dirLevel)
{
   int i = 0;
   while(GDS[i].EntryType)
   {
      // Checks if if current GDS is FileAndDirectoryEntry (FileDir - 0x85)
      // and the next GenericDirectoryStructure (i+1) is a StreamExtensionEntry (StreamExt - 0xc0)
      // and the one after that (i+2) is the FileNamEntry (FileName - 0xc1)
      if(GDS[i].InUse && GDS[i].EntryType == FileDir && GDS[i+1].EntryType == StreamExt && GDS[i+2].EntryType == FileName)
      {
         FileAttributes *fileAttributes = (FileAttributes *)((void *)&GDS[i] + FILE_ATTRIBUTE_OFFSET);
         StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)&GDS[i+1];
         FileNameEntry *fileNameEntry = (FileNameEntry *)&GDS[i+2];

         printFileName( (char *)fileNameEntry->FileName, streamExtEntry->NameLength, dirLevel );

         // If the attribute of the file is a directory, then recursively call this function to print its
         // contents, using its corresponding cluster, and increasing the directory level
         if(fileAttributes->Directory)
         {
            GDS_t *subGDS = findCluster(streamExtEntry->FirstCluster, fp, clustInfo);
            printDirectory(subGDS, fp, clustInfo, dirLevel+1);
         }
      }

      i++;
   }
}

/* === End of Utility Functions to Interact with the EXFAT === */



/* === Functions to be used by the extfat.c ===*/

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

   // Goes to the first directory entry entry
   GDS_t *GDS = findCluster(MB->FirstClusterOfRootDirectory, fp, clustInfo);

   // Finds the FileAndDirEntry (0x85) of the fileToDelete
   GDS = findFileAndDirEntry(GDS, fileToDelete, fp, clustInfo);

   // Used to check if the target file is a directory
   FileAttributes *fileAttributes = (FileAttributes *)((void *)GDS + FILE_ATTRIBUTE_OFFSET);

   // If GDS is NULL, the findFileAndDirEntry failed to find the file (return -1)
   // If the entry is a directory, then do not delete it and return 1
   if (GDS == NULL)
   {
      return -1;
   }
   else if(fileAttributes->Directory)
   {
      return 1;
   }

   /* If GDS is a FileAndDirectoryEntry(0x85 EntryType), then the
    * next entry after that is the StreamExtentionEntry. */
   StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)&GDS[1];

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
         clearFATChainAndData(fp, fd, file->FAT, clustInfo, streamExtEntry->FirstCluster);
      }
      else // there is only one cluster of file data
      {
         clearCluster(fd, fp, streamExtEntry->FirstCluster, clustInfo);
      }
   }

   /* turnOffInUseBits starts at the FileAndDirEntry, and turns off InUse bits
    * of all directory entries until it hits the last FileNameEntry. */
   turnOffInUseBits(fd, fp, GDS, numOfFilenameEntries);

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

/* === End of Functions to be used by the extfat.c ===*/