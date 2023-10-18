gcc %1 -o program.exe -O1 -Wall -std=c99 -Wno-missing-braces -L ./lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
program.exe