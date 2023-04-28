#include "searchExtfat.h"
#include "printExtfat.h"

/* ===== Additional internal helper functions ===== */

/* Prints the filename inside the exFAT image with the proper amount of tabs
 * depending on the dirLevel. */
void printFileName(char *ptrToFilename, int lengthOfName, int dirLevel)
{
    // Prints the proper number of tabs dependidng on dirLevel
    for (int i = 0; i < dirLevel; i++)
    {
        printf("\t");
    }

    // Gets the filename from exFAT into a standard C string. +1 is for '\0'.
    char *filename = malloc((lengthOfName + 1) * sizeof(char));
    fetchNameFromExtFAT(filename, ptrToFilename, lengthOfName);

    printf("%s\n", filename);
    free(filename);
}

/* ===== End of Additional internal helper functions ===== */


/* ===== Functions declared in printExtfat.h ===== */

/* Recursively traverses the exFAT image directory entries to print all files
 * and directories out in a tabbed format. Uses printFileName() to print the names. */
void printDirectory(GDS_t *GDS, void *fp, ClusterInfo clustInfo, int dirLevel)
{
    int i = 0;
    while (GDS[i].EntryType)
    {
        // Checks if if current GDS is FileAndDirectoryEntry (FileDir - 0x85)
        // and the next GenericDirectoryStructure (i+1) is a StreamExtensionEntry (StreamExt - 0xc0)
        // and the one after that (i+2) is the FileNamEntry (FileName - 0xc1)
        if (GDS[i].InUse && GDS[i].EntryType == FileDir && GDS[i + 1].EntryType == StreamExt && GDS[i + 2].EntryType == FileName)
        {
            FileAttributes *fileAttributes = (FileAttributes *)((void *)&GDS[i] + FILE_ATTRIBUTE_OFFSET);
            StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)&GDS[i + 1];
            FileNameEntry *fileNameEntry = (FileNameEntry *)&GDS[i + 2];

            printFileName((char *)fileNameEntry->FileName, streamExtEntry->NameLength, dirLevel);

            // If the attribute of the file is a directory, then recursively call this function to print its
            // contents, using its corresponding cluster, and increasing the directory level
            if (fileAttributes->Directory)
            {
                GDS_t *subGDS = findCluster(streamExtEntry->FirstCluster, fp, clustInfo);
                printDirectory(subGDS, fp, clustInfo, dirLevel + 1);
            }
        }

        i++;
    }
}

char printcontent(GDS_t *GDS, void *fp, ClusterInfo clustInfo, char *Filename, char *output)
{
    int i = 0;
    char Filenameinimage[100] = "";
    char *c1ptr = NULL;
    char resultcase = 'i';

    while (GDS[i].EntryType)
    {
        // Checks if if current GDS is FileAndDirectoryEntry (0x85)
        // and the next GDS_ture (i+1) is a StreamExtensionEntry (0xc0)
        // and the one after that (i+2) is the FileNamEntry (0xc1)

        if (GDS[i].InUse && GDS[i].EntryType == 0x85 && GDS[i + 1].EntryType == 0xc0 && GDS[i + 2].EntryType == 0xc1)
        {
            FileAttributes *fileAttributes = (FileAttributes *)((void *)&GDS[i] + FILE_ATTRIBUTE_OFFSET);
            StreamExtensionEntry *streamExtEntry = (StreamExtensionEntry *)&GDS[i + 1];
            memset(Filenameinimage, '\0', sizeof(Filenameinimage));

            c1ptr = (char *)&(GDS[i + 2]);
            c1ptr += 2;
            for (int j = 0; j < streamExtEntry->NameLength; j++)
            {
                Filenameinimage[j] = *c1ptr;

                c1ptr += 2;
            }
            // printf("(%s)(%s)",Filename, Filenameinimage);

            if (strcmp(Filename, Filenameinimage) == 0 && fileAttributes->Directory != 1)
            {
                // printf("File found");
                resultcase = 'f'; // indicate the file is found
                FILE *fpout = fopen(output, "w");
                GDS_t *contentcluster = findCluster(streamExtEntry->FirstCluster, fp, clustInfo);
                FileNameEntry *content = NULL;
                int w = 0;

                while (contentcluster[w].EntryType)
                {
                    content = (FileNameEntry *)(void *)&contentcluster[w];
                    for (int z = 0; z < (int)sizeof(GDS_t); z++)
                    {
                        //   printf("%c", (*content).words[z]);
                        fputc((*content).FileName[z], fpout);
                    }
                    w++;
                }
                fclose(fpout);
                return resultcase;
            }
            if (strcmp(Filename, Filenameinimage) == 0 && fileAttributes->Directory == 1)
            {
                // printf("Try to open a directory\n");
                resultcase = 'd'; // indicate try to open a directory
                return resultcase;
            }

            // If the attribute of the file is a directory, then recursively call this function to print its
            // contents, using its corresponding cluster, and increasing the directory level
            if (fileAttributes->Directory)
            {
                GDS_t *subGDS = findCluster(streamExtEntry->FirstCluster, fp, clustInfo);
                resultcase = printcontent(subGDS, fp, clustInfo, Filename, output);
            }
        }
        i++;
    }
    return resultcase;
}

/* ===== End of Functions declared in printExtfat.h ===== */