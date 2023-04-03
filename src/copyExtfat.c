#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <getopt.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "copyExtfat.h"
#include "extfat.h"

void mmapCopy(char *input_file, char *output_file)
{
      int fd = open(input_file, O_RDWR);
      
   if (fd == -1)
   {
      perror("file open ");
      exit(0);
   }

   // Take the pointer returned from mmap() and turn it into
   // a structure that understands the layout of the data
   Main_Boot *MB = (Main_Boot *)mmap(NULL,
                                     sizeof(Main_Boot),
                                     PROT_READ,
                                     MAP_PRIVATE,
                                     fd,
                                     0); // note the offset

   if (MB == (Main_Boot *)-1)
   {
      perror("error from mmap:");
      exit(0);
   }

   // below is change made in main
   struct stat statbuf;

   if (fstat(fd, &statbuf) < 0)
   {
      perror("fstat");
      exit(1);
   }

   if (mapOutput(fd, statbuf.st_size, output_file) != -1)
   {
      printf("Copied Succesfully!\n");
   }


   // unmap the file
   if (munmap(MB, sizeof(Main_Boot)) == -1)
   {
      perror("error from unmap:");
      exit(0);
   }
   // close the file
   if (close(fd))
   {
      perror("closeStat:");
   }
   fd = 0;

   
}


int mapOutput(int fdread, size_t size, char *output) // Function that maps the read memory into outputFile
{
   char *reading;
   char *puting;
   char write_byte = 'B';
   int fout = open(output, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);             
   reading = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fdread, 0);     //Passing in the read file 
   FILE *fp = fopen(output, "wb");

   printf("Size of before output file is %ld\n",ftell(fp));
   fseek(fp, size - 1, SEEK_END);                              //Seek to the end of the file and write a byte there using Fwrite to mark the size
   fwrite(&write_byte, sizeof(char), 1, fp);
   printf("Size of output file after writing a byte is %ld\n",ftell(fp));
   fseek(fp, 0, SEEK_SET);
   fclose(fp);                                                 //Close the file 
   puting = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fout, 0);         //Using MMAP to access output file
   printf("the size is %ld\n", size);
 
   if (fdread < 0)      //Error checking 
   {
      perror("OutputFile");
      return -1;
   }
   if (fout < 0)   //Error Checking
   {
      perror("OutputFile");
      return -1;
   }

   if (reading == MAP_FAILED)   //Error Checking
   {
      perror("Map failed");
      return -1;
   }
   if (puting == MAP_FAILED)     //Error Checking
   {
      perror("Map failed");
      return -1;
   }
   memcpy(puting, reading, size);     //Map memory over

   if (msync(puting, size, MS_SYNC) == -1) 
   {
        perror("msync error");
        return -1;
   }
   if (munmap(reading, size) < 0)
   {
      perror("unmap");
      return -1;
   }
   if (munmap(puting, size) < 0)
   {
      perror("unmap");
      return -1;
   }
   if (close(fout))
   {
      perror("Close :");
   }

   return 0;
}