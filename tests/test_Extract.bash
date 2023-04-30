echo "System Extraction Accuracy Test"

bash ./tests/clean_up.bash
bash ./examples/create_image.bash 

./extfat -i test.image -o output.txt -x mmap.c
diff output.txt /workspaces/cse3310_s004_group_18/examples/mmap.c
#Diff returns a value of 0 when identical so I check if a return code of 0 is returned here
if [ $? -eq 0 ]; then
    echo "Files are identical"
    result=0;
else
    echo "Files are different"
    result=1;
fi


echo "Result is $result"
exit $result

