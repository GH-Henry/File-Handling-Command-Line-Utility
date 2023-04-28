#include "unitTests.h"

MunitResult test_mmapCopy()
{
    fileInfo inputFileInfo = initFileInfoStruct("test.image");
    char *outputFilename = "test145114141.image";
    mmapCopy(&inputFileInfo, outputFilename);
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
   return MUNIT_OK;
}

// MunitResult test_printName()
// {
//     char buffer[256];
//     FILE* tempFile = fopen("test.image", "rw");
//     FILE* oldStdout = stdout;
//     stdout = tempFile;

//     char *name = "John";
//     int lengthOfName = strlen(name);
//     int dirLevel = 2;
//     printName(name, lengthOfName, dirLevel);

//     fclose(tempFile);
//     stdout = oldStdout;

//     const char *expectedOutput = "\t\tJohn\n";
//     munit_assert_string_equal(buffer, expectedOutput);

//     return MUNIT_OK;
// }

MunitResult test_freeFileInfoStruct()
{
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
    return MUNIT_OK;
}

MunitResult test_randomFunction()
{
    //to be done later :3
    return MUNIT_OK;
}

MunitTest tests[] = 
{
    {"/test_mmapCopy", test_mmapCopy, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    //{"/test_printName", test_printName, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_writeByteInFile", test_writeByteInFile, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_freeFileInfoStruct", test_freeFileInfoStruct, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_randomFunction", test_randomFunction, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)])
{
    MunitSuite suite = {"/tests", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};
    printf("Success\n");
    return munit_suite_main(&suite, NULL, argc, argv);
}
