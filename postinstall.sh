#!/usr/bin/env bash

if [ -n "$(node lib/nodegit.js 2>&1 | grep libstdc++)" ]; then
	echo "[ERROR] Seems like you the latest libstdc++ is missing on your system!"
	echo ""
	echo "On Ubuntu you can install it using:"
	echo ""
	echo "$ sudo add-apt-repository ppa:ubuntu-toolchain-r/test"
	echo "$ sudo apt-get update"
	echo "$ sudo apt-get install libstdc++-4.9-dev"
	exit 1
fi