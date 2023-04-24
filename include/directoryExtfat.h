#pragma once

// Takes in the pointer to a Main_Boot struct and prints its
// corrisponding directory and files.
void printAllDirectoriesAndFiles(void *pointerToMB);
void printfilecontent(void *fp, char *Filename, char *outputfilename);
void printAllDirectoriesAndFiles(void *fp);