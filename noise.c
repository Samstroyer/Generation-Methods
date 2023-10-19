#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>

/*
    This file is for creating my own type of perlin noise, inspired by perlin noise.
    This is not finalised but is a basic, just a proof of concept for me that I can make it :)

    It will work by creating a 2D array with random integers, then the coordinates you put in will be lerped between those values.
    Can of course be smoothened!

    This is not as easy as it seems - Midway update
*/

typedef struct ValueRange
{
    int min;
    int max;
} ValueRange;

#define WINDOW_WIDTH (short)800
#define WINDOW_HEIGHT (short)800
#define COLOR_TILE_SIZE (char)5
#define MAP_SIZE (char)20
#define MAP_TILE_VALUE_RANGE \
    (ValueRange) { .min = 0, .max = 9 }

int GetValueFromMap(double x, double y, int map[MAP_SIZE][MAP_SIZE])
{
    int index_x = (int)floor(x);             //% MAP_SIZE;
    int index_y = (int)floor(y);             //% MAP_SIZE;
    int move_towards_index_x = (int)ceil(x); //% MAP_SIZE;
    int move_towards_index_y = (int)ceil(y); //% MAP_SIZE;

    int value = map[index_x][index_y]; // = floor(Lerp(map[index_x][index_y], map[move_towards_index_x][move_towards_index_y], x - index_x));

    if (index_x != move_towards_index_x)
    {
        value += Lerp(value, map[move_towards_index_x][index_y], x - index_x);
    }
    else
    {
        int new = index_x - 1 < 0 ? MAP_SIZE - 1 : index_x - 1;
        value += map[new][index_y];
    }
    value /= 2;

    if (index_y != move_towards_index_y)
    {
        value += Lerp(value, map[index_x][move_towards_index_y], y - index_y);
    }
    else
    {
        int new = index_y - 1 < 0 ? MAP_SIZE - 1 : index_y - 1;
        value += map[index_x][new];
    }
    value /= 2;

    if (index_x != move_towards_index_x && index_y != move_towards_index_y)
    {
        value += Lerp(value, map[move_towards_index_x][move_towards_index_y], (y - index_y + x - index_x) / 2);
        value /= 2;
    }

    return value;
}

int main()
{
    SetTargetFPS(20);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My own 2D noise - procedural warp");

    int map[MAP_SIZE][MAP_SIZE];

    for (int y = 0; y < MAP_SIZE; y++)
    {
        for (int x = 0; x < MAP_SIZE; x++)
        {
            map[x][y] = GetRandomValue(MAP_TILE_VALUE_RANGE.min, MAP_TILE_VALUE_RANGE.max);
        }
    }

    Color color_map[WINDOW_WIDTH / COLOR_TILE_SIZE][WINDOW_HEIGHT / COLOR_TILE_SIZE];

    for (int y = 0; y < WINDOW_HEIGHT / COLOR_TILE_SIZE; y++)
    {
        for (int x = 0; x < WINDOW_WIDTH / COLOR_TILE_SIZE; x++)
        {
            int test = GetValueFromMap(x * 0.1, y * 0.1, map);
            color_map[x][y] = (Color){255, 255, 255, Remap(test, MAP_TILE_VALUE_RANGE.min, MAP_TILE_VALUE_RANGE.max, 0, 255)};
        }
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        for (int y = 0; y < WINDOW_HEIGHT / COLOR_TILE_SIZE; y++)
        {
            for (int x = 0; x < WINDOW_WIDTH / COLOR_TILE_SIZE; x++)
            {
                DrawRectangle(x * COLOR_TILE_SIZE, y * COLOR_TILE_SIZE, COLOR_TILE_SIZE, COLOR_TILE_SIZE, color_map[x][y]);
            }
        }

        // Shows all map values
        // for (int y = 0; y < MAP_SIZE; y++)
        // {
        //     for (int x = 0; x < MAP_SIZE; x++)
        //     {
        //         DrawText(TextFormat("%i", map[x][y]), x * WINDOW_WIDTH / MAP_SIZE, y * WINDOW_WIDTH / MAP_SIZE, 20, GREEN);
        //     }
        // }

        EndDrawing();
    }

    return 0;
}