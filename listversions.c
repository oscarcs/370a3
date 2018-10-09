/**
 * listversions.c
 * osim082
 * Usage: ./listversions filename
 *      Lists the versions of the file called 'filename'.
 *      The filename is expected to be a file in the mount directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char* argv[]) {
    
    // Get the name of the file to list versions on:
    const char* filename;
    if (argc == 2) {
        filename = argv[1];
    }
    else {
        printf("Incorrect number of arguments supplied.\n");
        exit(-1);
    }

    // Get the long filename
    char* long_filename = malloc(12 + sizeof(filename));
    strcpy(long_filename, ".versiondir/");
    strcat(long_filename, filename);

    // Check the file exists:
    int result = access(long_filename, F_OK);
    if (result == -1) {
        printf("File '%s' not found.\n", filename);
        exit(-1);
    }

    // Get the version files and print them:
    for (int i = 1; i <= 6; i++) {
        
        char* version_filename = malloc(12 + sizeof(filename) + 2);
        char* pretty_version_filename = malloc(sizeof(filename) + 2);
        sprintf(version_filename, ".versiondir/%s.%i", filename, i);
        sprintf(pretty_version_filename, "%s.%i", filename, i);

        int result = access(version_filename, F_OK);
        if (result == 0) {
            printf("%s\n", pretty_version_filename);
        }

        free(version_filename);
        free(pretty_version_filename);
    }

    free(long_filename);
}