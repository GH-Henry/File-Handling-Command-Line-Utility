#!/bin/bash
#Here I overwrite a byte in so Verify should return "Not the same" If it does then this test passes else it will not pass and return 1
echo "Verify Test"
IntactSum=$(./extfat -i test.image -v)
IntactOutput="Main Boot and Backup Boot checksums are the same."
if [[ $IntactOutput == *"$IntactSum"* ]]; then
    result=0
else
    result=1
fi

gcc -Wall overwrite.c
./a.out
./extfat -i test.image -v
output=$(./extfat -i test.image -v)
errorMessage="Main Boot and Backup Boot checksums are not the same."
#Check to see if output contains the error message
if [[ $output == *"$errorMessage"* ]]; then
    result=0
else
    result=1
fi
echo "Result is $result "
exit $result
