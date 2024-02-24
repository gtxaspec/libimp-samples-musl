#!/bin/bash

# Check if the -clean flag is set
if [[ "$1" == "-clean" ]]; then
	MAKE_CMD="make distclean"
	rm -f common/musl_shim.o
else
	MAKE_CMD="make"
fi

# Define the list of base directories
folders=("T20" "T21" "T23" "T30" "T31")

# Iterate over each folder
for folder in "${folders[@]}"; do
    echo "Processing folder: $folder"

    # Change to the folder and remember to return later
    pushd "$folder"

    # libimp
    if [ -d "libimp" ]; then
        pushd "libimp"
        $MAKE_CMD
        popd # Return from libimp
    else
        echo "libimp directory does not exist in $folder"
    fi

    # libsysutils
    if [ -d "libsysutils" ]; then
        pushd "libsysutils"
        $MAKE_CMD
        popd # Return from libsysutils
    else
        echo "libsysutils directory does not exist in $folder"
    fi

    popd # Return to the starting directory
done

echo "All done."
