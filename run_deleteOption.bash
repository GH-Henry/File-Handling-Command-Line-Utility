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
touch /tmp/d/helloThisWIllbeASuperDuperLongFileNameBecauseLongFileNamesAreSuperDuperCool
cp sample_files/A.txt /tmp/d/A.txt
cp sample_files/B.txt /tmp/d/B.txt
cp examples/fread.c /tmp/d/fread.c
cp sample_files/C.txt /tmp/d/C.txt
sync

# hexdump -C test.image > hdump1
echo ">>> Print"
./extfat -i test.image -d

echo ">>> Delete fread.c"
./extfat -i test.image -D fread.c
sudo umount $LOOP
sudo losetup -d $LOOP
sudo losetup $LOOP test.image
sudo mount $LOOP /tmp/d
# hexdump -C test.image > hdump2

echo ">>> Print"
./extfat -i test.image -d

echo ">>> Copying D.txt"
cp sample_files/D.txt /tmp/d/D.txt
sync
# hexdump -C test.image > hdump3

echo ">>> Print"
./extfat -i test.image -d

echo ">>> Deleting SuperLong Filename"
./extfat -i test.image -D helloThisWIllbeASuperDuperLongFileNameBecauseLongFileNamesAreSuperDuperCool
sudo umount $LOOP
sudo losetup -d $LOOP
sudo losetup $LOOP test.image
sudo mount $LOOP /tmp/d
# hexdump -C test.image > hdump4

echo ">>> Print"
./extfat -i test.image -d

echo ">>> mkdir and mp.c in dir"
mkdir /tmp/d/dir
cp /tmp/d/mmap.c /tmp/d/dir/mp.c
sync
# hexdump -C test.image > hdump5

echo ">>> Print"
./extfat -i test.image -d

echo ">>> Attempt to delete dir"
./extfat -i test.image -D dir
sudo umount $LOOP
sudo losetup -d $LOOP
sudo losetup $LOOP test.image
sudo mount $LOOP /tmp/d
# hexdump -C test.image > hdump6

echo ">>> Print"
./extfat -i test.image -d

echo ">>> Attempt to delete dir"
./extfat -i test.image -D dir
sudo umount $LOOP
sudo losetup -d $LOOP
sudo losetup $LOOP test.image
sudo mount $LOOP /tmp/d
# hexdump -C test.image > hdump7

echo ">>> Print"
./extfat -i test.image -d

echo ">>> Delete mp.c"
./extfat -i test.image -D mp.c
sudo umount $LOOP
sudo losetup -d $LOOP
sudo losetup $LOOP test.image
sudo mount $LOOP /tmp/d
# hexdump -C test.image > hdump8

echo ">>> Attempt to delete dir"
./extfat -i test.image -D dir
sudo umount $LOOP
sudo losetup -d $LOOP
sudo losetup $LOOP test.image
sudo mount $LOOP /tmp/d
# hexdump -C test.image > hdump9

echo ">>> Print"
./extfat -i test.image -d

echo ">>> Delete D.txt"
./extfat -i test.image -D D.txt
sudo umount $LOOP
sudo losetup -d $LOOP
sudo losetup $LOOP test.image
sudo mount $LOOP /tmp/d
# hexdump -C test.image > hdump10

echo ">>> Print"
./extfat -i test.image -d

echo ">>> make mp.c"
cp /tmp/d/mmap.c /tmp/d/dir/mp.c
sync
# hexdump -C test.image > hdump11

echo ">>> Print"
./extfat -i test.image -d

sudo umount $LOOP
sudo losetup -d $LOOP