#!/bin/sh
# This needed to link libpng
# LDFLAGS=-L/usr/local/lib might be needed for Intel Mac
./configure --enable-static LDFLAGS=-L/opt/homebrew/lib
