#!/bin/bash
echo "Copy Files Test"

result1=($(/usr/bin/md5sum test.image))

#checks for the accuracy between the md5sum hashes of the two files. 
./extfat -i test.image -o test2.image -c


result2=($(/usr/bin/md5sum test2.image))

# clean up
$(rm test2.image)

if [ "${result1[0]}" = "${result2[0]}" ]; then
    result=0
else
    result=1
fi
exit $result
