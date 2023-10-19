#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

/*
    THIS CELLULAR AUTOMATA USES MOORE NEIGHBOOR
    As I understand there is Moore Neighboor or Von Neumann Neighboor.
    I at least use Moore's way
*/

typedef struct SignedChar2
{
    signed char x, y;
} SignedChar2;

SignedChar2 offsets[8] = {
    (SignedChar2){-1, -1},
    (SignedChar2){0, -1},
    (SignedChar2){1, -1},
    (SignedChar2){-1, 0},
    (SignedChar2){1, 0},
    (SignedChar2){-1, 1},
    (SignedChar2){0, 1},
    (SignedChar2){1, 1},
};

#define WINDOW_WIDTH (short)800
#define WINDOW_HEIGHT (short)800
#define TILE_SIZE (short)10
#define TILES_HEIGHT (WINDOW_HEIGHT / TILE_SIZE)
#define TILES_WIDTH (WINDOW_WIDTH / TILE_SIZE)

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cellular automata - Procedural");

    bool new_map[TILES_WIDTH][TILES_HEIGHT];
    bool map[TILES_WIDTH][TILES_HEIGHT];

    for (int x = 0; x < TILES_WIDTH; x++)
    {
        for (int y = 0; y < TILES_HEIGHT; y++)
        {
            map[x][y] = GetRandomValue(0, 1);
            new_map[x][y] = map[x][y];
        }
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();

        for (int x = 0; x < TILES_WIDTH; x++)
        {
            for (int y = 0; y < TILES_HEIGHT; y++)
            {
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, (Color){255 * map[x][y], 0, 0, 255});
            }
        }

        DrawFPS(10, 10);

        EndDrawing();

        for (int x = 0; x < TILES_WIDTH; x++)
        {
            for (int y = 0; y < TILES_HEIGHT; y++)
            {
                char neighboor_count = 0;
                for (int i = 0; i < 9; i++)
                {
                    int index_x = (x + offsets[i].x + 80) % TILES_WIDTH;
                    int index_y = (y + offsets[i].y + 80) % TILES_HEIGHT;

                    neighboor_count += map[index_x][index_y];
                }

                if (map[x][y] == false && neighboor_count == 3)
                {
                    new_map[x][y] = true;
                }
                else
                {
                    if (neighboor_count < 2)
                    {
                        new_map[x][y] = false;
                    }
                    else if (neighboor_count < 4)
                    {
                        new_map[x][y] = true;
                    }
                    else
                    {
                        new_map[x][y] = false;
                    }
                }
            }
        }

        for (int x = 0; x < TILES_WIDTH; x++)
        {
            for (int y = 0; y < TILES_HEIGHT; y++)
            {
                map[x][y] = new_map[x][y];
            }
        }
    }

    return 0;
}