#include "unitTests.h"

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

MunitResult test_printName()
{
    char buffer[256];
    FILE* tempFile = freopen(buffer, (long int*)sizeof(buffer), 'w');
    FILE* oldStdout = stdout;
    stdout = tempFile;

    char *name = "John";
    int lengthOfName = strlen(name);
    int dirLevel = 2;
    printName(name, lengthOfName, dirLevel);

    fclose(tempFile);
    stdout = oldStdout;

    const char *expectedOutput = "\t\tJohn\n";
    munit_assert_string_equal(buffer, expectedOutput);

    return MUNIT_OK;
}

void printName(char *charPtr, int legnthOfName, int dirLevel)
{
    // Prints the proper number of tabs dependidng on dirLevel
    for(int i = 0; i < dirLevel; i++)
    {
        printf("\t");
    }

    for(int i = 0; i < legnthOfName; i++)
    {
        // In the event that the name is longer than 15 characters, then the next byte
        // that charPtr will point at will be 0xc1 to signal another FileNameEntry,
        // thus must offset by 2 to get to a printable character.
        if(*charPtr == (char)0xc1)
        {
            charPtr += 2;
        }
        printf("%c", *charPtr);
        charPtr += 2; // offset by 2 to get to the next character in the name
    }

    printf("\n");
}

MunitTest tests[] = 
{
    {"/test_printName", test_printName, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_writeByteInFile", test_writeByteInFile, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)])
{
    MunitSuite suite = {"/tests", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};
    printf("Success\n");
    return munit_suite_main(&suite, NULL, argc, argv);
}
