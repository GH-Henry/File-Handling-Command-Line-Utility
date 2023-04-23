#include "unitTests.h"

MunitTest tests[] = 
{
    {"/test_printName", test_printName, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_writeByteInFile", test_writeByteInFile, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_parseArgs", test_parseArgs, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

MunitResult test_writeByteInFile(const MunitParameter params[], void* fixture)
{
    char* outputFilename = "test_output.bin";
    size_t offset = 10;

    writeByteInFile(outputFilename, offset);

    FILE* fp = fopen(outputFilename, "rb");
    fseek(fp, offset - 1, SEEK_SET);
    char readByte;
    fread(&readByte, sizeof(char), 1, fp);
    munit_assert_char(readByte, ==, 'B');

    fclose(fp);
    remove(outputFilename);

    return MUNIT_OK;
}

MunitResult test_printName(const MunitParameter params[], void* user_data)
{
    char buffer[256];
    FILE* tempFile = fmemopen(buffer, sizeof(buffer), "w");
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

MunitResult test_parseArgs(const MunitParameter params[], void* user_data) {
    const char* argv[] = { "program_name", "-x", "invalid_option" };
    int argc = sizeof(argv) / sizeof(char*);

    char* input_file = NULL;
    char* output_file = NULL;
    char* expected_error = "unrecognized option '--invalid_option'\n";
    argument_struct_t actual_error = parseArgs(argc, argv);

    munit_assert_null(input_file);
    munit_assert_null(output_file);
    assert_string_equal(actual_error, expected_error);

    return MUNIT_OK;
}

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)])
{
    MunitSuite suite = {"/tests", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};
    return munit_suite_main(&suite, NULL, argc, argv);
}
