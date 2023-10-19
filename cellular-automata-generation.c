#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

/*
    THIS CELLULAR AUTOMATA USES MOORE NEIGHBOOR
    As I understand there is Moore Neighboor or Von Neumann Neighboor.
    I at least use Moore's way
*/

typedef struct Char2
{
    signed char x, y;
} Char2;

Char2 offsets[8] = {
    (Char2){-1, -1},
    (Char2){0, -1},
    (Char2){1, -1},
    (Char2){-1, 0},
    (Char2){1, 0},
    (Char2){-1, 1},
    (Char2){0, 1},
    (Char2){1, 1},
};

#define WINDOW_WIDTH (short)800
#define WINDOW_HEIGHT (short)800
#define TILE_SIZE (short)10
#define TILES_HEIGHT (WINDOW_HEIGHT / TILE_SIZE)
#define TILES_WIDTH (WINDOW_WIDTH / TILE_SIZE)
#define CHANCE (unsigned char)45

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cellular automata - Procedural");
    SetTargetFPS(3);

    bool new_map[TILES_WIDTH][TILES_HEIGHT];
    bool map[TILES_WIDTH][TILES_HEIGHT];

    for (int x = 0; x < TILES_WIDTH; x++)
    {
        for (int y = 0; y < TILES_HEIGHT; y++)
        {
            map[x][y] = GetRandomValue(0, 100) <= CHANCE ? 1 : 0;
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
                char neighboor_count = map[x][y];
                for (int i = 0; i < 9; i++)
                {
                    int index_x = (x + offsets[i].x + 80) % TILES_WIDTH;
                    int index_y = (y + offsets[i].y + 80) % TILES_HEIGHT;

                    neighboor_count += map[index_x][index_y];
                }

                if (neighboor_count < 5)
                {
                    new_map[x][y] = false;
                }
                else
                {
                    new_map[x][y] = true;
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