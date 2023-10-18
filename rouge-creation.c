#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h>
#include <stdlib.h>

/*
    This file is for creating rouge-like dungeon creation.
    This is not finalised but is a basic, just a proof of concept for me that I can make it :)
*/

typedef enum TILE_TYPE
{
    EMPTY,
    FLOOR,
    PATH,
    WALL,
    SPAWNABLE_ENTITY,
    SPAWNABLE_LOOT,
    COUNT
} TILE_TYPE;

Color colors[6] = {GREEN, BROWN, YELLOW, DARKBROWN, GOLD, GRAY};

typedef struct Tile
{
    unsigned char type;
} Tile;

#define WINDOW_WIDTH (short)800
#define WINDOW_HEIGHT (short)800
#define TILE_SIZE (char)10
#define ROOM_MIN_SIZE (char)5
#define ROOM_MAX_SIZE (char)11
#define ROOMS_MAX (char)10

int main()
{
    SetTargetFPS(5);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rouge Dungeons - procedural");

    long tile_amount = (WINDOW_WIDTH / TILE_SIZE) * (WINDOW_HEIGHT / TILE_SIZE);

    printf("Amount of tiles: %i\n", tile_amount);

    Tile map[tile_amount];
    for (int i = 0; i < tile_amount; i++)
        map[i].type = EMPTY;

    unsigned char rooms_placed = 0;

    Vector2 *room_positions = (Vector2 *)calloc(ROOMS_MAX, sizeof(Vector2));

    bool paths_placed = false;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);

        // Draw the map
        for (int y = 0; y < WINDOW_HEIGHT / TILE_SIZE; y++)
        {
            for (int x = 0; x < WINDOW_WIDTH / TILE_SIZE; x++)
            {
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, colors[map[x + (y * WINDOW_WIDTH / TILE_SIZE)].type]);
                // DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
            }
        }

        for (int i = 0; i < rooms_placed; i++)
        {
            DrawCircle(room_positions[i].x * TILE_SIZE, room_positions[i].y * TILE_SIZE, 5, colors[i]);
        }

        if (rooms_placed < ROOMS_MAX)
        {
            // Get a random position and draw the random position
            short random_pos_x = GetRandomValue(0, WINDOW_WIDTH / TILE_SIZE);
            short random_pos_y = GetRandomValue(0, WINDOW_HEIGHT / TILE_SIZE);

            DrawRectangle(random_pos_x * TILE_SIZE, random_pos_y * TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);
            DrawRectangleLines(random_pos_x * TILE_SIZE, random_pos_y * TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);

            // HERE WE PLACE THE ROOM
            // We place the room from the bottom left, easiest approach
            int room_height = GetRandomValue(ROOM_MIN_SIZE, ROOM_MAX_SIZE);
            int room_width = GetRandomValue(ROOM_MIN_SIZE, ROOM_MAX_SIZE);

            int lowest_index = random_pos_y - (room_height);
            int highest_index = random_pos_x + room_width + (random_pos_y * WINDOW_WIDTH / TILE_SIZE);

            if (lowest_index >= 0 && highest_index < tile_amount && (random_pos_x + room_width < WINDOW_WIDTH / TILE_SIZE))
            {
                // Look if the spot is taken or not
                bool occupied = false;
                for (int x = 0; x < room_width; x++)
                {
                    for (int y = 0; y < room_height; y++)
                    {
                        int x_point = (random_pos_x + x);
                        int y_point = ((random_pos_y - y) * (WINDOW_WIDTH / TILE_SIZE));
                        if (map[x_point + y_point].type != EMPTY)
                        {
                            occupied = true;
                        }
                    }
                }

                // If it was not taken, take it
                if (!occupied)
                {
                    for (int x = 1; x < room_width - 1; x++)
                    {
                        for (int y = 1; y < room_height - 1; y++)
                        {
                            int x_point = (random_pos_x + x);
                            int y_point = ((random_pos_y - y) * (WINDOW_WIDTH / TILE_SIZE));
                            map[x_point + y_point].type = FLOOR;
                            room_positions[rooms_placed] = (Vector2){x_point, random_pos_y};
                        }
                    }

                    rooms_placed++;
                }
            }
        }
        else if (!paths_placed)
        {
            for (int i = 0; i < ROOMS_MAX - 1; i++)
            {
                Vector2 startPos = room_positions[i];
                Vector2 endPos = room_positions[i + 1];
                Vector2 pathVectors = Vector2Subtract(startPos, endPos);

                printf("Have to create path: \n%f,%f", pathVectors.x, pathVectors.y);
                // pathVectors = Vector2Divide(pathVectors, (Vector2){10, 10});

                for (int x = 0; x < abs(pathVectors.x); x++)
                {
                    if (pathVectors.x >= 0)
                    {
                        int index = (startPos.x - x) + (startPos.y * (WINDOW_WIDTH / TILE_SIZE));
                        map[index].type = PATH;
                    }
                    else
                    {
                        int index = (startPos.x + x) + (startPos.y * (WINDOW_WIDTH / TILE_SIZE));
                        map[index].type = PATH;
                    }
                }

                for (int y = 0; y < abs(pathVectors.y); y++)
                {
                    if (pathVectors.y >= 0)
                    {
                        int index = (startPos.x - pathVectors.x) + (startPos.y * (WINDOW_WIDTH / TILE_SIZE)) - (y * (WINDOW_WIDTH / TILE_SIZE));
                        map[index].type = PATH;
                    }
                    else
                    {
                        int index = (startPos.x - pathVectors.x) + (startPos.y * (WINDOW_WIDTH / TILE_SIZE)) + (y * (WINDOW_WIDTH / TILE_SIZE));
                        map[index].type = PATH;
                    }
                }
            }
            int i = room_positions[ROOMS_MAX - 1].x + (room_positions[ROOMS_MAX - 1].y * WINDOW_WIDTH / TILE_SIZE);
            map[i].type = PATH;
            paths_placed = true;
        }
        else
        {
            // DO SOMETHING LIKE A STAR TO FIND THE SMALLEST ROUTE?
        }

        DrawFPS(10, 10);

        EndDrawing();
    }

    return 0;
}