#!/bin/bash

# Development build script for melonDS
# Run this in MSYS2 terminal after making changes

echo "Building melonDS..."
cmake --build build

if [ $? -eq 0 ]; then
    echo "Build successful! Copying to portable folder..."
    
    # Copy executable
    cp build/melonDS.exe melonDS-portable/
    
    echo "Ready to test! Run melonDS-portable/melonDS.exe"
    echo "Or test now? (y/n)"
    read -r response
    if [[ "$response" =~ ^([yY][eE][sS]|[yY])$ ]]; then
        cd melonDS-portable
        ./melonDS.exe
    fi
else
    echo "Build failed!"
fi
