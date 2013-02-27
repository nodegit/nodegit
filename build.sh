#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
git submodule update --init
rm -rf "$DIR/vendor/libgit2/build"
./configure && \
    make
