/**
 * rmversions.c
 * osim082
 * Usage: ./rmversions filename
 *      Permanently remove all of the versions except the current version of 
 *      the file denoted by 'filename'.
 */

#include <stdio.h>
#include <unistd.h>

int main(int argc, const char* argv[]) {
    
    // Get the filename
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
    if (access(long_filename, F_OK) == -1) {
        printf("File '%s' not found.\n", filename);
        exit(-1);
    }

    // Get the version files and delete all of them:
    for (int i = 1; i <= 6; i++) {
        
        char* version_filename = malloc(12 + sizeof(filename) + 2);
        sprintf(version_filename, ".versiondir/%s.%i", filename, i);
        
        if (access(version_filename, F_OK) == 0) {
            remove(version_filename);
        }

        free(version_filename);
    }

    free(long_filename);
}
