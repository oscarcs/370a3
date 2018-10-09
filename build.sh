#!/bin/sh

# Build the C utils. 
gcc listversions.c -ggdb -o listversions
gcc mkcurrent.c -ggdb -o mkcurrent
gcc catversion.c -ggdb -o catversion
# gcc rmversions.c -ggdb -o rmversions
# gcc shutdownversions.c -ggdb -o shutdownversions