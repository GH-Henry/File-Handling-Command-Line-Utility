#include "unitTests.h"

MunitResult test_initFileInfoStruct()
{
    system("bash ./examples/create_image.bash");
    char *filename = "test.image";
    fileInfo file = initFileInfoStruct(filename);

    munit_assert_int(file.fd, !=, -1);
    munit_assert_string_equal(file.fileName, filename);

    system("rm test.image");
    return MUNIT_OK;
}

MunitResult test_mmapCopy()
{
    // Runs bash script to create an image file
    system("bash ./examples/create_image.bash");

    fileInfo inputFileInfo = initFileInfoStruct("test.image");
    char *outputFilename = "test2.image";

    mmapCopy(&inputFileInfo, outputFilename);

    // diff returns 0 if both test.image and test2.image are the same
    int result = system("diff test.image test2.image");
    munit_assert_int(result, ==, 0); // Checks if diff returned a 0

    // Deletes image files and unmounts
    system("rm test.image test2.image");
    system("bash ./examples/unmount_images.bash");
    return MUNIT_OK;
}

MunitResult test_writeByteInFile()
{
    char *outputFilename = "test_output.bin";
    size_t offset = 1 << 10;

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

    system("rm test_output.bin");
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
    system("bash ./examples/create_image.bash");
    fileInfo file = initFileInfoStruct("test.image");
    freeFileInfoStruct(&file);

    // Checks to see if freeFileInfoStruct set everything to zero
    munit_assert_null(file.mainBoot);
    munit_assert_null(file.backupBoot);
    munit_assert_null(file.FAT);
    munit_assert_null(file.Data);
    munit_assert_null(file.fileName);
    munit_assert_int(file.fd, ==, 0);
    munit_assert_int(file.SectorSize, ==, 0);
    munit_assert_int(file.SectorsPerCluster, ==, 0);
    munit_assert_int(file.FileLength, ==, 0);
    munit_assert_long(file.size, ==, 0);

    // Deletes image files and unmounts
    system("rm test.image");
    system("bash ./examples/unmount_images.bash");
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

    argument_struct_t argStruct1 = parseArgs(0, NULL);

    munit_assert_null(argStruct1.inFile);
    munit_assert_null(argStruct1.outFile);
    munit_assert_null(argStruct1.delFile);
    munit_assert_null(argStruct1.extractFile);
    for(int i = 0; i < 6; i++)
    {
        munit_assert_false(argStruct1.flags[i]);
    }

    char *args[] = {"./extfat",
                    "-i",
                    "test.image",       //2
                    "-o",
                    "test2.image",      //4
                    "-v",
                    "-c",
                    "-d",
                    "-h",
                    "-x",
                    "targetExtract",    //10
                    "-D",
                    "targetDelete",     //12
                    NULL
                    };

    argument_struct_t argStruct2 = parseArgs(13, args);
    bool flagResult2[6] = {true, true, true, true, true, true};

    munit_assert_ptr_equal(argStruct2.inFile, args[2]);
    munit_assert_ptr_equal(argStruct2.outFile, args[4]);
    munit_assert_ptr_equal(argStruct2.extractFile, args[10]);
    munit_assert_ptr_equal(argStruct2.delFile, args[12]);

    munit_assert_memory_equal(sizeof(flagResult2), argStruct2.flags, flagResult2);

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

int main(int argc, char *argv[MUNIT_ARRAY_PARAM(argc + 1)])
{
    MunitSuite suite = {"/tests", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};
    return munit_suite_main(&suite, NULL, argc, argv);
}
