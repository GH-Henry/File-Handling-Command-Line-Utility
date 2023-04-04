#/bin/bash
# this script is designed to make images to assist in developing and
# testing the program(s)
# the '$' gets the result of a command/variable, LOOP is a variable
LOOP=$(sudo losetup -f) # gets the next free loop device and stores the result in LOOP
dd if=/dev/zero of=test.image count=1 bs=100M
sudo losetup $LOOP test.image
sudo /usr/sbin/mkexfatfs $LOOP
#
#
#
#
mkdir /tmp/d
sudo mount $LOOP /tmp/d
cp examples/mmap.c /tmp/d
sync
echo ">>> First Print"
./extfat -i test.image -d
mkdir /tmp/d/a
mkdir /tmp/d/a/b
mkdir /tmp/d/a/b/superLongDirNameOkayLetsGo
sync
echo ">>> Nested Directories"
./extfat -i test.image -d
cp -r include /tmp/d/
cp -r examples /tmp/d/
cp -r src /tmp/d/
sync
echo ">>> Printing Copied files from workspace"
./extfat -i test.image -d
touch /tmp/d/a/file1
touch /tmp/d/a/file2
touch /tmp/d/a/file3
touch /tmp/d/a/b/File10
touch /tmp/d/a/b/File11
touch /tmp/d/a/b/File12
touch /tmp/d/a/b/superLongDirNameOkayLetsGo/superDuperLongFileNameThatIsSuperLongAndItIsStillGoingIThinkItWillEndHereAndNow
sync
echo ">>> Multiple Files in Directories"
./extfat -i test.image -d
mkdir /tmp/d/a/b2
touch /tmp/d/a/b2/File20
touch /tmp/d/a/b2/File21
touch /tmp/d/a/b2/superLongFileNameRightHereRightNowLetsGo
touch /tmp/d/a/emptyDir
touch /tmp/d/a/b/OtherFile
mkdir /tmp/d/a/b/otherDirectory
touch /tmp/d/a/b/otherDirectory/file100
touch /tmp/d/a/b/otherDirectory/filenameW15char
touch /tmp/d/a/b/superLongDirNameOkayLetsGo/file999999999999
sync
echo ">>> Printing more files"
./extfat -i test.image -d
echo ">>> Unmounting and removing test.image"
sudo umount $LOOP
sudo losetup -d $LOOP
rm test.image
rm -rf /tmp/d