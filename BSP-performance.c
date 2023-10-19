#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"

#define WINDOW_WIDTH (short)800
#define WINDOW_HEIGHT (short)800
#define DEPTH (int)12 // all time record 30

long PowerOf(int base, int exp)
{
    long result = 1;

    for (int i = 0; i < exp; i++)
    {
        result *= base;
    }

    return result;
}

typedef struct shortRec
{
    short x;
    short y;
    short w;
    short h;
} shortRec;

shortRec *leaf_arr;
long index = 0;

void GenerateRooms(long depth, shortRec dividing_room)
{
    if (depth == DEPTH)
    {
        leaf_arr[index] = dividing_room;
        index++;
    }
    else
    {
        if (GetRandomValue(0, 1) < 1)
        {
            {
                shortRec r1 = (shortRec){dividing_room.x, dividing_room.y, dividing_room.w / 2, dividing_room.h};
                GenerateRooms(depth + 1, r1);
            }
            {
                shortRec r2 = (shortRec){dividing_room.x + dividing_room.w / 2, dividing_room.y, dividing_room.w / 2, dividing_room.h};
                GenerateRooms(depth + 1, r2);
            }
        }
        else
        {
            {
                shortRec r1 = (shortRec){dividing_room.x, dividing_room.y, dividing_room.w, dividing_room.h / 2};
                GenerateRooms(depth + 1, r1);
            }
            {
                shortRec r2 = (shortRec){dividing_room.x, dividing_room.y + dividing_room.h / 2, dividing_room.w, dividing_room.h / 2};
                GenerateRooms(depth + 1, r2);
            }
        }
    }
}

int main()
{
    leaf_arr = malloc(sizeof(shortRec) * PowerOf(2, DEPTH)); // calloc(PowerOf(2, DEPTH), sizeof(shortRec));

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "BSP - Procedural");

    GenerateRooms(0, (shortRec){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});
    // GenerateRooms(0, (shortRec){0, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT});
    // GenerateRooms(0, (shortRec){WINDOW_WIDTH / 2, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT});

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(WHITE);

        // for (int i = 0; i < PowerOf(2, DEPTH); i++)
        // {
        //     DrawRectangle(leaf_arr[i].x, leaf_arr[i].y, leaf_arr[i].w, leaf_arr[i].h, RED);
        //     DrawRectangleLines(leaf_arr[i].x, leaf_arr[i].y, leaf_arr[i].w, leaf_arr[i].h, (Color){0, 0, 0, 40});
        // }

        DrawFPS(10, 10);

        EndDrawing();
    }

    return 0;
}