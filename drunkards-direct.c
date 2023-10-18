#include "raylib.h"
#include "raymath.h"

#define WINDOW_WIDTH (short)800
#define WINDOW_HEIGHT (short)800
#define TILE_SIZE (short)10

typedef struct Tile
{
    bool visited;
} Tile;

Vector2 directions[4] = {
    (Vector2){1, 0},
    (Vector2){0, 1},
    (Vector2){-1, 0},
    (Vector2){0, -1}};

int main()
{
    SetTargetFPS(1);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Drunkards walk - Procedural");

    Tile map[WINDOW_WIDTH / TILE_SIZE][WINDOW_HEIGHT / TILE_SIZE];

    for (int y = 0; y < WINDOW_WIDTH / TILE_SIZE; y++)
    {
        for (int x = 0; x < WINDOW_HEIGHT / TILE_SIZE; x++)
        {
            map[x][y].visited = false;
        }
    }

    short steps = 0;
    short steps_max = 50;
    short used_drunkards = 0;
    int amount_of_drunkards = 50000;

    while (used_drunkards < amount_of_drunkards)
    {
        Vector2 index = (Vector2){WINDOW_WIDTH / TILE_SIZE / 2, WINDOW_HEIGHT / TILE_SIZE / 2};
        while (steps < steps_max)
        {
            map[(int)index.x][(int)index.y].visited = true;

            index = Vector2Add(index, directions[GetRandomValue(0, 3)]);

            if (index.x >= WINDOW_WIDTH / TILE_SIZE)
            {
                index.x--;
            }
            else if (index.x < 0)
            {
                index.x++;
            }

            if (index.y >= WINDOW_WIDTH / TILE_SIZE)
            {
                index.y--;
            }
            else if (index.y < 0)
            {
                index.y++;
            }

            steps++;
        }
        used_drunkards++;
        steps = 0;
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GREEN);

        for (int y = 0; y < WINDOW_WIDTH / TILE_SIZE; y++)
        {
            for (int x = 0; x < WINDOW_HEIGHT / TILE_SIZE; x++)
            {
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, (Color){255, 0, 0, 255 * map[x][y].visited});
                // DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
            }
        }

        EndDrawing();
    }

    return 0;
}