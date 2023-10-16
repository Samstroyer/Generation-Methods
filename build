#!/bin/bash

if cc main.c -framework IOKit -framework Cocoa -framework OpenGL $(pkg-config --libs --cflags raylib) -o program -O2;
then
    ./program
fi