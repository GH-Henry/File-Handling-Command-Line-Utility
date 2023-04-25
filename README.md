# Extfat utils is a utility to manipulate extfat images.

### Command Options:
```bash
-i: Input file { } (required)
-o: Output File
-h: Print Help
-c: Copy
-v: Verify Checksum
-d: Delete Extfat
-x: Extract Extfat { }
-p: Print Directory
```
{ }: *Requires file name*

### Example Invocations:
```
./extfat -h
./extfat -i test.image -o test2.image -c
./extfat -i test.image -o test2.image -v
./extfat -i test.image -o test2.image -d
./extfat -i test.image -o test2.image -x test.image
./extfat -i test.image -c -v -p
./extfat -i test.image -c -v -p -x test.image 
```
## Internals

### Build Process
```bash
% make clean
% make

To run the tests
% make tests
% make unit_tests

```

### Examples
There is a directory of examples.  They are compiled by default using the makefile
Before you can run the example programs, you need to have an file image.

```bash
% bash /examples/create_image.bash
% ./examples/fread
% ./examples/mmap
```

### References

https://pawitp.medium.com/notes-on-exfat-and-reliability-d2f194d394c2

https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification

https://uta.service-now.com/selfservice?id=ss_kb_article&sys_id=KB0011414

https://nemequ.github.io/munit/#download

https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html

https://www.freecodecamp.org/news/how-to-use-git-and-github-in-a-team-like-a-pro/

https://en.wikipedia.org/wiki/ExFAT

https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax
