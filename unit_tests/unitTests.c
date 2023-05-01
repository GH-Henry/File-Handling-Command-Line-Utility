#include "unitTests.h"

fileInfo initFileInfoStruct(char *fileName)
{
   fileInfo file = {};
   file.fd = open(fileName, O_RDWR);
   if (file.fd == -1)
   {
      perror("file open");
      exit(0);
   }

   struct stat statbuf;
   if (fstat(file.fd, &statbuf))
   {
      perror("stat of file");
      exit(0);
   }

   file.size = statbuf.st_size; // add size to the thing

   void *fp = (void *)mmap(NULL,
                           file.size,
                           PROT_READ,
                           MAP_PRIVATE,
                           file.fd,
                           0); // note the offset

   if (fp == (void *)-1)
   {
      perror("mmap");
      exit(0);
   }

   // first, is the Main Boot record
   Main_Boot *MB = (Main_Boot *)fp;
   file.mainBoot = MB;

   int bytesPerSector = 2 << (file.mainBoot->BytesPerSectorShift - 1); // Can be added to a property of the struct
   
   void *fp_ptr = (void*)(intptr_t)fp;
   file.backupBoot = (Main_Boot *)(fp_ptr + 12 * bytesPerSector);
   file.SectorSize = bytesPerSector;
   file.FAT = (uint32_t *)(fp_ptr + (file.mainBoot->FatOffset * bytesPerSector));
   file.fileName = fileName;

   return file;
}

MunitResult test_initFileInfoStruct()
{
    system("bash ./examples/create_image.bash");
    char *filename = "test.image";
    fileInfo file = initFileInfoStruct(filename);

    munit_assert_int(file.fd, !=, -1);
    munit_assert_string_equal(file.fileName, filename);

    return MUNIT_OK;
}

MunitResult test_mmapCopy()
{
    system("bash ./examples/create_image.bash");
    fileInfo inputFileInfo = initFileInfoStruct("test.image");
    char *outputFilename = "test145114141.image";
    if( strcmp(inputFileInfo.fileName, outputFilename) == 0 )
    {
        printf("Same input and output file name, nothing will happen.\n");
        return -1;
    }

    size_t size = inputFileInfo.size;
    void *source;
    void *destination;
    
    int fout = open(outputFilename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fout < 0) //Error Checking
    {
        perror("OutputFile");
        return -1;
    }

    source = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, inputFileInfo.fd, 0); //Passing in the read file
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
    system("rm test145114141.image");
    system("rm test.image");
    return MUNIT_OK;
}

MunitResult test_writeByteInFile() 
{
   char *outputFilename = "test_output.bin";
   size_t offset = 10;

   writeByteInFile(outputFilename, offset);

   FILE *fp = fopen(outputFilename, "rb");
   fseek(fp, 0, SEEK_END);
   size_t fileSize = ftell(fp);
   fclose(fp);

   // Check that file size is equal to the offset
   munit_assert_int(fileSize, ==, offset);

   // Check that byte at the offset is equal to 'B'
   fp = fopen(outputFilename, "rb");
   fseek(fp, offset - 1, SEEK_SET);
   char readByte;
   fread(&readByte, sizeof(char), 1, fp);
   fclose(fp);

   munit_assert_char('B', ==, readByte);
   //printf("%d", MUNIT_OK);
   system("rm test_output.bin");
   return MUNIT_OK;
}

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

MunitResult test_freeFileInfoStruct()
{
    system("bash ./examples/create_image.bash");
    fileInfo file = initFileInfoStruct("test.image");
    // unmap the file
    if (munmap(file.mainBoot, file.size))
    {
        perror("error from unmap:");
        exit(0);
    }

    // close the file
    if (close(file.fd))
    {
        perror("close");
    }
    file.fd = 0;
    system("rm test.image");
    return MUNIT_OK;
}

MunitResult test_verifyBoot()
{
   system("bash ./examples/create_image.bash");
   fileInfo file = initFileInfoStruct("test.image");

   uint32_t mbrChecksum = BootChecksum((uint8_t*) file.mainBoot, (short) file.SectorSize);
   uint32_t bbrChecksum = BootChecksum((uint8_t*) file.backupBoot, (short) file.SectorSize);

   munit_assert_uint32(mbrChecksum, ==, bbrChecksum);

   system("rm test.image");
   return MUNIT_OK;
}

MunitResult test_parseArgs()
{
   system("bash ./examples/create_image.bash");

   int argcTest = 8;
   char *argvTest[] = {"-i", "test.image", "-o", "test2.image", "-c", "-v", "-d", "-h"};
   argument_struct_t argumentsTest = parseArgs(argcTest, argvTest);

   system("rm test.image");
   system("rm test2.image");
   return MUNIT_OK;
}

MunitTest tests[] = 
{
    {"/test_initFileInfoStruct", test_initFileInfoStruct, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_mmapCopy", test_mmapCopy, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_writeByteInFile", test_writeByteInFile, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_freeFileInfoStruct", test_freeFileInfoStruct, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_verifyBoot", test_verifyBoot, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_parseArgs", test_parseArgs, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)])
{
    MunitSuite suite = {"/tests", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};
    system("rm test");
    system("make clean");
    return munit_suite_main(&suite, NULL, argc, argv);
}
