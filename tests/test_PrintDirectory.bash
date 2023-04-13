#!/bin/bash

echo "Directory Test"

# Create a disk image

# Display the directory tree of the disk image

sudo umount /tmp/d
rm -rf /tmp/d
bash ./examples/create_image.bash
mkdir /tmp/d/dir1
touch /tmp/d/dir1/file
sync
./extfat -i test.image -d 
#I put mmap.c dir1 and file inside the image
output=$(./extfat -i test.image -d)

# Check if the output contains the expected file names
if [[ $output == *"file"* ]] && [[ $output == *"mmap.c"* ]] && [[ $output == *"dir1"* ]] ; then
    result=0
else
    result=1
fi

echo "Result is $result"
exit $result
