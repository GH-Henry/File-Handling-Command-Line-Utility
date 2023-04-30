echo "Deleted while Mounted Test"
#This test will test the case when the file is deleted while image is mounted (Meaning it will still show up despite you clearing it)
bash ./tests/clean_up.bash 
bash ./examples/create_image.bash 
./extfat -i test.image -D mmap.c
output=$(ls /tmp/d)
#We use the ls command, if the output still contains mmap.c then we pass the test
if [[ $output == *"mmap.c"* ]]; then
    result=0
else
    result=1
fi
echo "Result is $result "
exit $result