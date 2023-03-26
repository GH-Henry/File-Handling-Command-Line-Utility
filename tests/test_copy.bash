#!/bin/bash
echo "Copy Files Test"

result1=($(/usr/bin/md5sum test.image))

# this should be a call to extfat -c <infile> <outfile>
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
