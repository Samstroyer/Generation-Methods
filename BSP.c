#include <stdio.h>
#include "raylib.h"

#define WINDOW_WIDTH (short)800
#define WINDOW_HEIGHT (short)800
#define DEPTH (int)16

int PowerOf(int base, int exp)
{
    int result = 1;

    for (int i = 0; i < exp; i++)
    {
        result *= base;
    }

    return result;
}

Rectangle *leaf_arr;
int index = 0;

void GenerateRooms(int depth, Rectangle dividing_room)
{
    if (depth == DEPTH)
    {
        // Save leaf
        leaf_arr[index] = dividing_room;
        index++;
        return;
    }
    else
    {
        // Decide vertical or horizontal
        if (GetRandomValue(0, 1) < 1)
        {
            {
                Rectangle r1 = (Rectangle){dividing_room.x, dividing_room.y, dividing_room.width / 2, dividing_room.height};
                GenerateRooms(depth + 1, r1);
            }
            {
                Rectangle r2 = (Rectangle){dividing_room.x + dividing_room.width / 2, dividing_room.y, dividing_room.width / 2, dividing_room.height};
                GenerateRooms(depth + 1, r2);
            }
        }
        else
        {
            {
                Rectangle r1 = (Rectangle){dividing_room.x, dividing_room.y, dividing_room.width, dividing_room.height / 2};
                GenerateRooms(depth + 1, r1);
            }
            {
                Rectangle r2 = (Rectangle){dividing_room.x, dividing_room.y + dividing_room.height / 2, dividing_room.width, dividing_room.height / 2};
                GenerateRooms(depth + 1, r2);
            }
        }
        return;
    }
}

int main()
{
    Rectangle leafs[PowerOf(2, DEPTH)];
    leaf_arr = leafs;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "BSP - Procedural");

    GenerateRooms(0, (Rectangle){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(WHITE);

        for (int i = 0; i < PowerOf(2, DEPTH); i++)
        {
            DrawRectangleRec(leafs[i], RED);
            DrawRectangleLinesEx(leafs[i], 2, (Color){0, 0, 0, 30});
        }

        EndDrawing();
    }

    return 0;
}