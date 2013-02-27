#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Check for presence of .git directory, update submodules if present
if [ -d "$DIR/.git" ]; then
    echo "Initializing & updating submoules"
    git submodule init
    git submodule update
else
    echo "Cloning libgit2"
    git clone git://github.com/libgit2/libgit2.git "$DIR/vendor/libgit2"
    cd "$DIR/vendor/libgit2"
    git checkout v0.15.0
    cd $DIR
fi

if [ -d "$DIR/vendor/libgit2/build" ]; then
    echo "Removing previous build directory"
    rm -rf "$DIR/vendor/libgit2/build"
fi

./configure && \
    make
