#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

#define WINDOW_WIDTH (int)800
#define WINDOW_HEIGHT (int)800
#define ROOM_SIZE __UINT8_MAX__ // Room size is the size of the world - a room as it is "travel through"
#define TILE_SIZE (int)40
#define BASE_PLAYER_SPEED (int)200
#define PLAYER_RADIUS (TILE_SIZE / 2)

typedef enum TILE_TYPE
{
    TILE_AIR,

    TILE_FG_DIRT,
    TILE_BG_DIRT,

    TILE_FG_STONE,
    TILE_BG_STONE,

    TILE_FG_IRON,
    TILE_BG_IRON,

    TILE_COUNT,
} TILE_TYPE;

typedef struct Tile
{
    TILE_TYPE type;
    unsigned short hit_points;
} Tile;

Tile tiles_info[TILE_COUNT] = {
    (Tile){TILE_AIR, 0},
    (Tile){TILE_FG_DIRT, 1},
    (Tile){TILE_BG_DIRT, 0},
    (Tile){TILE_FG_STONE, 2},
    (Tile){TILE_BG_STONE, 0},
    (Tile){TILE_FG_IRON, 3},
    (Tile){TILE_BG_IRON, 0},
};

Color tile_colors[TILE_COUNT] = {
    BLUE,
    BROWN,
    DARKBROWN,
    GRAY,
    DARKGRAY,
    LIGHTGRAY,
    DARKGRAY,
};

typedef struct Char2
{
    char x;
    char y;
} Char2;

typedef struct Player
{
    float x;
    float y;
    signed char health;
} Player;

void GenerateOre(Tile map[ROOM_SIZE][ROOM_SIZE], int amount_of_patches, int walks, int steps)
{
    Vector2 directions[4] = {
        (Vector2){-1, 0}, // left
        (Vector2){1, 0},  // right
        (Vector2){0, -1}, // up
        (Vector2){0, 1},  // down
    };

    int point_x;
    int point_y;

    // p for patches
    for (int p = 0; p < amount_of_patches; p++)
    {
        do
        {
            point_x = GetRandomValue(0, ROOM_SIZE - 1);
            point_y = GetRandomValue(25, ROOM_SIZE - 1);
        } while (map[point_x][point_y].hit_points > 0);

        // d for drunkard
        for (int d = 0; d < walks; d++)
        {
            // s for steps
            for (int s = 0; s < steps; s++)
            {
                map[point_x][point_y] = tiles_info[TILE_FG_IRON];

                char index = GetRandomValue(0, 4);
                point_x += directions[index].x;
                point_y += directions[index].y;

                if (point_x < 0 || point_x >= ROOM_SIZE)
                    point_x -= directions[index].x;
                if (point_y < 15 || point_x >= ROOM_SIZE)
                    point_y -= directions[index].y;
            }
        }
    }
}

void CarveTunnels(Tile map[ROOM_SIZE][ROOM_SIZE], int passes, char airiness)
{
    char y_offset = 15;
    char chance = airiness;

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

    // -15 because it is air!
    bool cellular_map[ROOM_SIZE][ROOM_SIZE - y_offset];
    bool prev_cellular_map[ROOM_SIZE][ROOM_SIZE - y_offset];

    for (int y = y_offset; y < ROOM_SIZE; y++)
    {
        for (int x = 0; x < ROOM_SIZE; x++)
        {
            cellular_map[x][y - y_offset] = GetRandomValue(0, 100) <= chance ? true : false;
            prev_cellular_map[x][y - y_offset] = cellular_map[x][y - y_offset];
        }
    }

    for (int p = 0; p < passes; p++)
    {

        for (int y = y_offset; y < ROOM_SIZE; y++)
        {
            for (int x = 0; x < ROOM_SIZE; x++)
            {
                char neighboor_count = cellular_map[x][y - y_offset];
                for (int i = 0; i <= 8; i++)
                {
                    int index_x = (x + offsets[i].x);
                    int index_y = (y + offsets[i].y);

                    if (index_x < 0 || index_x > ROOM_SIZE)
                        continue;
                    if (index_y < y_offset || index_y > ROOM_SIZE)
                        continue;

                    neighboor_count += cellular_map[index_x][index_y - y_offset];
                }

                if (neighboor_count < 5)
                {
                    prev_cellular_map[x][y] = false;
                }
                else
                {
                    prev_cellular_map[x][y] = true;
                }
            }
        }

        for (int x = 0; x < ROOM_SIZE; x++)
        {
            for (int y = y_offset; y < ROOM_SIZE; y++)
            {
                cellular_map[x][y - y_offset] = prev_cellular_map[x][y - y_offset];
            }
        }
    }

    for (int x = 0; x < ROOM_SIZE; x++)
    {
        for (int y = y_offset; y < ROOM_SIZE; y++)
        {
            map[x][y] = cellular_map[x][y - y_offset] ? (Tile){map[x][y].type + 1, 0} : map[x][y];
        }
    }
}

void MapGenerator(Tile map[ROOM_SIZE][ROOM_SIZE], int difficulty)
{
    for (int x = 0; x < ROOM_SIZE; x++)
    {
        for (int y = 0; y < ROOM_SIZE; y++)
        {
            map[x][y] = y < 15 ? tiles_info[TILE_AIR] : y < 18 ? tiles_info[TILE_FG_DIRT]
                                                               : tiles_info[TILE_FG_STONE];
        }
    }

    CarveTunnels(map, 5, 48);
    GenerateOre(map, 100, 2, 3);
}

int main()
{
    Player p = (Player){125 * TILE_SIZE, 12 * TILE_SIZE, 100};

    {
        SetRandomSeed(&p);
    }

    Tile map[ROOM_SIZE][ROOM_SIZE];
    MapGenerator(map, 1);

    Camera2D cam = (Camera2D){(Vector2){WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2}, (Vector2){p.x, p.y}, 0, 1};

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Game, with procedural!");
    SetTargetFPS(500);

    Rectangle player_hitbox = (Rectangle){p.x - PLAYER_RADIUS * 0.45, p.y - PLAYER_RADIUS * 0.45, TILE_SIZE * 0.9, TILE_SIZE * 0.9};
    while (!WindowShouldClose())
    {
        // Input for movement
        Vector2 movement = (Vector2){0, 0};
        float speed = BASE_PLAYER_SPEED * GetFrameTime() * (IsKeyDown(KEY_LEFT_SHIFT) * 1 + 1);
        movement.x += IsKeyDown(KEY_D) * speed;
        movement.x -= IsKeyDown(KEY_A) * speed;
        movement.y += IsKeyDown(KEY_S) * speed;
        movement.y -= IsKeyDown(KEY_W) * speed;

        // Check map and character collision
        bool can_move_x = true;
        bool can_move_y = true;
        bool can_move_current = true;
        for (int x = 0; x < ROOM_SIZE; x++)
        {
            for (int y = 0; y < ROOM_SIZE; y++)
            {
                if (map[x][y].type == TILE_AIR || map[x][y].hit_points <= 0)
                    continue;
                if (cam.target.x - cam.offset.x / 4 > (x * TILE_SIZE) + TILE_SIZE || cam.target.x + cam.offset.x / 4 < x * TILE_SIZE)
                    continue;
                if (cam.target.y - cam.offset.y / 4 > (y * TILE_SIZE) + TILE_SIZE || cam.target.y + cam.offset.y / 4 < y * TILE_SIZE)
                    continue;
                Rectangle block = (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                if (CheckCollisionRecs((Rectangle){player_hitbox.x + movement.x, player_hitbox.y + movement.y, player_hitbox.width, player_hitbox.height}, block))
                {
                    can_move_current = false;
                }
                if (CheckCollisionRecs((Rectangle){player_hitbox.x + movement.x, player_hitbox.y, player_hitbox.width, player_hitbox.height}, block))
                {
                    can_move_x = false;
                }
                if (CheckCollisionRecs((Rectangle){player_hitbox.x, player_hitbox.y + movement.y, player_hitbox.width, player_hitbox.height}, block))
                {
                    can_move_y = false;
                }
            }
        }

        // Unhindered movement
        if (can_move_current)
        {
            p.x += movement.x;
            p.y += movement.y;
        }
        else
        {
            // Hindered movement y
            if (can_move_x)
            {
                p.x += movement.x;
            }
            // Hindered movement x
            if (can_move_y)
            {
                p.y += movement.y;
            }
        }

        // Update hitbox
        player_hitbox = (Rectangle){p.x - PLAYER_RADIUS * 0.45, p.y - PLAYER_RADIUS * 0.45, TILE_SIZE * 0.9, TILE_SIZE * 0.9};

        // Update the camera
        cam.target = (Vector2){p.x, p.y};

        BeginDrawing();
        BeginMode2D(cam);

        ClearBackground(WHITE);

        // Draw the map
        for (int x = 0; x < ROOM_SIZE; x++)
        {
            for (int y = 0; y < ROOM_SIZE; y++)
            {
                if (cam.target.x - cam.offset.x > (x * TILE_SIZE) + TILE_SIZE || cam.target.x + cam.offset.x < x * TILE_SIZE)
                    continue;
                if (cam.target.y - cam.offset.y > (y * TILE_SIZE) + TILE_SIZE || cam.target.y + cam.offset.y < y * TILE_SIZE)
                    continue;
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, tile_colors[map[x][y].type]);
                DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, (Color){0, 0, 0, 30});
            }
        }

        DrawRectangleRec(player_hitbox, RED);

        EndMode2D();

        DrawFPS(10, 10);
        EndDrawing();
    }

    return 0;
}