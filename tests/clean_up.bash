#!/bin/bash
if [ -d "/tmp/d" ] 
then
   sudo umount /dev/loop2
   sudo losetup -d /dev/loop2
   rm -rf /tmp/d
fi
if [ -d "test.image" ] 
then
   rm test.image
fi