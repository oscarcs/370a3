/**
 * rmversions.c
 * osim082
 * Usage: ./shutdownversions
 *      Delete .versiondir.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, const char* argv[]) {
    system("rm -r ./.versiondir");
    system("fuser mount -u mount");
}