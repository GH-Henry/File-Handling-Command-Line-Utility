echo "Checks if system deletes"

bash ./tests/clean_up.bash
bash ./examples/create_image.bash 
sudo umount /tmp/d
./extfat -i test.image -D mmap.c


if [ -e /workspaces/cse3310_s004_group_18/tmp/d/mmap.c ]; then
    result=1
else
    result=0
fi

echo "Result is $result"
exit $result

