#!/bin/bash

if cc $1 -framework IOKit -framework Cocoa -framework OpenGL $(pkg-config --libs --cflags raylib) -o program -O2;
then
    ./program
fi