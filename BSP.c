#include <stdio.h>
#include "raylib.h"

#define WINDOW_WIDTH (short)800
#define WINDOW_HEIGHT (short)800
#define DEPTH (int)4 // Make a power of 2 because binary

typedef struct Branch
{
    Rectangle rec;
    struct Branch *branches[2];
} Branch;

int PowerOf(int base, int exp)
{
    int result = 1;

    for (int i = 0; i < exp; i++)
    {
        result *= base;
    }

    return result;
}

int main()
{
    Branch rooms[PowerOf(2, DEPTH)];

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "BSP - Procedural");

    return 0;
}