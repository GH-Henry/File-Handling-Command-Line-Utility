#include <stdio.h>
#include <string.h>

#include "copyExtfat.h"
#include "util.h"

void writeByteInFile(char *outputFilename, size_t offset)
{
   char writeByte = 'B';
   FILE *fp = fopen(outputFilename, "wb");
   printf("Size of before output file is %ld\n", ftell(fp));

   //Seek to the end of the file and write a byte there using Fwrite to mark the size
   fseek(fp, offset - 1, SEEK_END); 
   fwrite(&writeByte, sizeof(char), 1, fp);

   printf("Size of output file after writing a byte is %ld\n", ftell(fp));

   fseek(fp, 0, SEEK_SET);
   fclose(fp); //Close the file
}

int mmapCopy(fileInfo *inputFileInfo, char *outputFilename) // Function that maps the read memory into outputFile
{
   if( strcmp(inputFileInfo->fileName, outputFilename) == 0 )
   {
      printf("Same input and output file name, nothing will happen.\n");
      return -1;
   }

   size_t size = inputFileInfo->size;
   void *source;
   void *destination;
   
   int fout = open(outputFilename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
   if (fout < 0) //Error Checking
   {
      perror("OutputFile");
      return -1;
   }

   source = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, inputFileInfo->fd, 0); //Passing in the read file
   if (source == MAP_FAILED) //Error Checking
   {
      perror("Map failed");
      return -1;
   }

   destination = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fout, 0); //Using MMAP to access output file
   if (destination == MAP_FAILED) //Error Checking
   {
      perror("Map failed");
      return -1;
   }

   writeByteInFile(outputFilename, size);

   printf("the size is %ld\n", size);
   
   memcpy(destination, source, size);     //Map memory over

   if (msync(destination, size, MS_SYNC) == -1) 
   {
        perror("msync error");
        return -1;
   }

   if (munmap(source, size) < 0)
   {
      perror("unmap");
      return -1;
   }
   if (munmap(destination, size) < 0)
   {
      perror("unmap");
      return -1;
   }

   if (close(fout))
   {
      perror("Close");
   }

   return 0;
}