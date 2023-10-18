#!/bin/bash

if cc $1 -o program -framework IOKit -framework Cocoa -framework OpenGL $(pkg-config --libs --cflags raylib) -O2;
then
    ./program
fi