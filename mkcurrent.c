/**
 * mkcurrent.c
 * osim082
 * Usage: ./mkcurrent filename version
 *      Makes the version of 'filename' specified by 'version' the 
 *      current version.
 *      The version 'version' becomes the new version 1, and the remaining
 *      versions are pushed down the stack.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, const char* argv[]) {
    
    // Get the version and the filename: 
    const char* filename;
    const char* version_str;
    if (argc == 3) {
        filename = argv[1];
        version_str = argv[2];
    }
    else {
        printf("Incorrect number of arguments supplied.\n");
        exit(-1);
    }

    // Parse the version into a number:
    int target_version = atoi(version_str);

    // Get the file for the selected file:
    char* long_filename = malloc(12 + sizeof(filename));
    strcpy(long_filename, ".versiondir/");
    strcat(long_filename, filename);

    // Check the selected version actually exists:
    char* selected_version_filename = malloc(sizeof(long_filename) + 2);
    sprintf(selected_version_filename, "%s.%i", long_filename, target_version);

    if (access(selected_version_filename, F_OK) != 0) {
        printf("Selected version does not exist.\n");
        exit(-1);
    }

    // Shift the versions down, starting from the final version file:
    for (int i = 6; i >= 1; i--) {

        char* version_filename = malloc(sizeof(long_filename) + 2);
        sprintf(version_filename, "%s.%i", long_filename, i);

        char* next_version_filename = malloc(sizeof(long_filename) + 2);
        sprintf(next_version_filename, "%s.%i", long_filename, i + 1);

        if (access(version_filename, F_OK) == 0) {
            // rename the file at i into the file at i + 1:
            rename(version_filename, next_version_filename);
        }

        free(version_filename);
        free(next_version_filename);
    }

    // Copy the selected version file into the main file and the first 
    // version file:
    sprintf(selected_version_filename, "%s.%i", long_filename, target_version + 1); // note: i+1

    char* first_version_filename = malloc(sizeof(long_filename) + 2);
    sprintf(first_version_filename, "%s.1", long_filename);

    // There's no easy way to copy a file in C without reading in the file and 
    // writing it back out! So we cheat and use the shell...
    char* copy_command = malloc(2 + 1 + sizeof(selected_version_filename)
        + 1 + sizeof(first_version_filename));
    
    sprintf(copy_command, "cp %s %s", selected_version_filename, first_version_filename);
    system(copy_command);

    sprintf(copy_command, "cp %s %s", selected_version_filename, long_filename);
    system(copy_command);

    // Remove version 7, if it exists:
    char* version_7_filename = malloc(sizeof(filename) + 2);
    strcpy(version_7_filename, long_filename);
    strcat(version_7_filename, ".7");
    
    if (access(version_7_filename, F_OK) == 0) {
        remove(version_7_filename);
    }
    free(version_7_filename);

    // "Only YOU can prevent memory leaks!"
    free(selected_version_filename);
    free(first_version_filename);
    free(copy_command);
    free(long_filename);
}
