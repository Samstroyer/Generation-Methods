#include "raylib.h"
#include "raymath.h"

#define WINDOW_WIDTH (short)800
#define WINDOW_HEIGHT (short)800
#define TILE_SIZE (short)10

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cellular Automata - procedural");
    SetTargetFPS(1);

    bool map[WINDOW_WIDTH / TILE_SIZE][WINDOW_HEIGHT / TILE_SIZE];

    for (int y = 0; y < WINDOW_HEIGHT / TILE_SIZE; y++)
    {
        for (int x = 0; x < WINDOW_WIDTH / TILE_SIZE; x++)
        {
            map[x][y] = GetRandomValue(0, 1);
        }
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();

        for (int y = 0; y < WINDOW_HEIGHT / TILE_SIZE; y++)
        {
            for (int x = 0; x < WINDOW_WIDTH / TILE_SIZE; x++)
            {
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, (Color){255 * map[x][y], 0, 0, 255});
            }
        }

        DrawFPS(10, 10);

        EndDrawing();
    }

    return 0;
}