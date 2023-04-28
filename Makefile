# for this project, there is only one makefile
#
# this makefile is expected to be run from the project 
# root.
#
# all executables are expected to be ran from the project 
# root.
#
# this makefile will clean up when asked politely
#
# No intermediate .o files or libraries are created

CFLAGS=-Wall -Wextra -O0 -std=c17 -g3 -fsanitize=address -fsanitize=bounds-strict
# note address sanitizer "-fsanitize=address" is new. it can be
# removed from the makefile if it causes problems.

CSRCS  = src/copyExtfat.c src/deleteExtfat.c src/directoryExtfatUtility.c src/parseArgs.c src/printExtfat.c src/searchExtfat.c src/util.c
CSRCS += common/routines.c
CINCS  = -I./include
TESTSRC = unit_tests/unitTests.c unit_tests/munit/munit.c

all:mmap fread unit_tests extfat crc_example

# the utility that is a focus of this project
extfat: ${CSRCS} src/extfat.c
	${CC} ${CFLAGS} ${CINCS} -o $@ $^

# unit tests
unit_tests: munit_test

munit_test: ${TESTSRC} ${CSRCS}
	${CC} ${CFLAGS} -I./unit_tests/munit_example ${CINCS} -o $@ $^

# this test needs to be deleted once we get some real tests
# for the problem at hand
#munit_example:unit_tests/munit
#	${CC} ${CFLAGS} unit_tests/munit_example -I./unit_tests/munit_example ${CINCS} -o $@ $^

# requirements tests
system_tests: extfat
	bash tests/system_tests.bash

test_CheckOutputExists: extfat 
	bash tests/test_CheckOutputExists.bash

test_copy: extfat 
	bash tests/test_copy.bash 

test_ExitAfterHelp: extfat
	bash tests/test_ExitAfterHelp.bash

test_help: extfat
	bash tests/test_help.bash

test_input_message: extfat 
	bash tests/test_input_message.bash

test_PrintDirectory: extfat
	bash tests/test_PrintDirectory.bash

test_verify: extfat
	bash tests/test_verify.bash

test_MultipleFlags: extfat 
	bash tests/test_MultipleFlags.bash

test_NoCopyDest: extfat 
	bash tests/test_NoCopyDest.bash
# example code
mmap:examples/mmap.c  common/routines.c
	${CC} ${CFLAGS} ${CINCS} -o $@ $^

fread:examples/fread.c  
	${CC} ${CFLAGS} ${CINCS} -o $@ $^

crc_example:examples/crc_example.c
	${CC} ${CFLAGS} ${CINCS} -o $@ $^ -lz

# run tests
tests: run_unit_tests system_tests

run_unit_tests: munit_example
	./munit_example

run_tests:
	echo "here i would be running the requirements tests"

clean:
	-rm -f mmap fread munit_example extfat crc_example output munit_test test_output.bin overwrite.c