#!/bin/sh

# Build the C utils. 

# listversions
gcc listversions.c -ggdb -o listversions

# mkcurrent
gcc mkcurrent.c -ggdb -o mkcurrent

# catversion
# cc catversion.c -o catversion

# rmversions
# cc rmversions.c -o rmversions

# shutdownversions
# cc shutdownversions.c -o shutdownversions