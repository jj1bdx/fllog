#!/bin/sh
# This needed to link libpng
# -L/opt/homebrew/lib might be needed for M1
./configure --enable-static LDFLAGS=-L/usr/local/lib
