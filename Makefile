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

CINCS=-I./include

all:mmap fread unit_tests extfat crc_example

# the utility that is a focus of this project
extfat:src/extfat.c common/routines.c src/parseArgs.c src/copyExtfat.c
	${CC} ${CFLAGS} ${CINCS} -o $@ $^


# unit tests
unit_tests: munit_example

# this test needs to be deleted once we get some real tests
# for the problem at hand
munit_example:unit_tests/munit/example.c
	${CC} ${CFLAGS}  unit_tests/munit/munit.c -I./unit_tests/munit/ ${CINCS} -o $@ $^

# requirements tests


# example code
mmap:examples/mmap.c  
	${CC} ${CFLAGS} ${CINCS} -o $@ $^

fread:examples/fread.c  
	${CC} ${CFLAGS} ${CINCS} -o $@ $^

crc_example:examples/crc_example.c
	${CC} ${CFLAGS} ${CINCS} -o $@ $^ -lz

# run tests
tests: run_unit_tests run_tests

run_unit_tests: munit_example
	./munit_example

run_tests:
	echo "here i would be running the requirements tests"

clean:
	-rm -f mmap fread munit_example extfat crc_example
