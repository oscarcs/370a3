/**
 * catversion.c
 * osim082
 * Usage: ./catversion filename version
 *      Display the contents of the version of the given file onto the screen.
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

    // Get the long filename and check it exists
    char* long_filename = malloc(12 + sizeof(filename));
    strcpy(long_filename, ".versiondir/");
    strcat(long_filename, filename);
    
    if (access(long_filename, F_OK) == -1) {
        printf("File '%s' not found.\n", filename);
        exit(-1);
    }

    // Check the selected version actually exists:
    char* selected_version_filename = malloc(sizeof(long_filename) + 2);
    sprintf(selected_version_filename, "%s.%i", long_filename, target_version);

    if (access(selected_version_filename, F_OK) != 0) {
        printf("Selected version does not exist.\n");
        exit(-1);
    }

    // Cat the file
    char* cat_command = malloc(3 + 1 + sizeof(selected_version_filename));
    sprintf(cat_command, "cat %s", selected_version_filename);
    system(cat_command);

    free(long_filename);
    free(selected_version_filename);
}