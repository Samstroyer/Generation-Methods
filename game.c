#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"

#define WINDOW_WIDTH (int)800
#define WINDOW_HEIGHT (int)800
#define ROOM_SIZE __UINT8_MAX__ // Room size is the size of the world - a room as it is "travel through"
#define TILE_SIZE (int)30
#define BASE_PLAYER_SPEED (int)200
#define PLAYER_RADIUS (TILE_SIZE / 2)
#define MAX_POPUPS (short)30 // Overkill, but this is so that you never can overflow the message size :)
#define POPUP_TIMER_SECONDS (unsigned char)2
#define WORLD_GRAVITY (unsigned char)1

#pragma region TILES
typedef enum TILE_TYPE
{
    TILE_AIR,

    TILE_FG_DIRT,
    TILE_BG_DIRT,

    TILE_FG_STONE,
    TILE_BG_STONE,

    TILE_FG_IRON,
    TILE_BG_IRON,

    TILE_FG_COAL,
    TILE_BG_COAL,

    TILE_PORTAL,

    TILE_COUNT,
} TILE_TYPE;

char block_names[TILE_COUNT][6] = {
    "air"
    "dirt",
    "dirt",
    "stone",
    "stone",
    "iron",
    "iron",
    "coal",
    "coal",
    "portal",
};

typedef struct Tile
{
    TILE_TYPE type;
    float hit_points;
} Tile;

Tile tiles_info[TILE_COUNT] = {
    (Tile){TILE_AIR, 0},
    (Tile){TILE_FG_DIRT, 1},
    (Tile){TILE_BG_DIRT, 0},
    (Tile){TILE_FG_STONE, 2},
    (Tile){TILE_BG_STONE, 0},
    (Tile){TILE_FG_IRON, 3},
    (Tile){TILE_BG_IRON, 0},
    (Tile){TILE_FG_COAL, 2},
    (Tile){TILE_BG_COAL, 0},
    (Tile){TILE_PORTAL, 0},
};

Color tile_colors[TILE_COUNT] = {
    (Color){100, 100, 255, 255}, // own blue
    BROWN,
    DARKBROWN,
    GRAY,
    DARKGRAY,
    LIGHTGRAY,
    DARKGRAY,
    BLACK,
    DARKGRAY,
    PURPLE,
};
#pragma endregion TILES

#pragma region UPGRADES
typedef enum UPGRADE_TYPE
{
    UPGRADE_SPEED_MOVEMENT,
    UPGRADE_SPEED_MINING,
    UPGRADE_COUNT,
} UPGRADE_TYPE;

char upgrade_names[UPGRADE_COUNT][8] = {
    "boots",
    "pickaxe",
};

typedef struct Upgrade_Info
{
    char *material_name;
    int multiplier; // This multiplies from base value of 1
    UPGRADE_TYPE type;
} Upgrade_Info;

typedef struct Upgrade_Requirement_Info
{
    TILE_TYPE type;
    int amount;
} Upgrade_Requirement_Info;

typedef enum UPGRADE_LEVELS
{
    LEVELS_DEFAULT,
    LEVELS_ONE,
    LEVELS_TWO,
    LEVELS_THREE,
    LEVELS_COUNT,
} UPGRADE_LEVELS;

Upgrade_Info mining_upgrades[LEVELS_COUNT] = {
    (Upgrade_Info){
        .material_name = "Default",
        .type = UPGRADE_SPEED_MINING,
        .multiplier = 2,
    },
    (Upgrade_Info){
        .material_name = "Stone",
        .type = UPGRADE_SPEED_MINING,
        .multiplier = 3,
    },
    (Upgrade_Info){
        .material_name = "Coal",
        .type = UPGRADE_SPEED_MINING,
        .multiplier = 4.5f,
    },
    (Upgrade_Info){
        .material_name = "Iron",
        .type = UPGRADE_SPEED_MINING,
        .multiplier = 6,
    },
};

Upgrade_Requirement_Info mining_upgrades_requirements[LEVELS_COUNT] = {
    (Upgrade_Requirement_Info){},
    (Upgrade_Requirement_Info){.amount = 30, .type = TILE_FG_STONE},
    (Upgrade_Requirement_Info){.amount = 40, .type = TILE_FG_COAL},
    (Upgrade_Requirement_Info){.amount = 40, .type = TILE_FG_IRON},
};

Upgrade_Info movement_upgrades[LEVELS_COUNT] = {
    (Upgrade_Info){
        .material_name = "Default",
        .type = UPGRADE_SPEED_MOVEMENT,
        .multiplier = 1,
    },
    (Upgrade_Info){
        .material_name = "Dirt",
        .type = UPGRADE_SPEED_MOVEMENT,
        .multiplier = 1.4f,
    },
    (Upgrade_Info){
        .material_name = "Stone",
        .type = UPGRADE_SPEED_MOVEMENT,
        .multiplier = 1.8f,
    },
    (Upgrade_Info){
        .material_name = "Iron",
        .type = UPGRADE_SPEED_MOVEMENT,
        .multiplier = 2.2f,
    },
};

Upgrade_Requirement_Info movement_upgrades_requirements[LEVELS_COUNT] = {
    (Upgrade_Requirement_Info){},
    (Upgrade_Requirement_Info){.amount = 30, .type = TILE_FG_DIRT},
    (Upgrade_Requirement_Info){.amount = 40, .type = TILE_FG_STONE},
    (Upgrade_Requirement_Info){.amount = 25, .type = TILE_FG_IRON},
};

unsigned char mining_level = LEVELS_DEFAULT;
unsigned char movement_level = LEVELS_DEFAULT;
#pragma endregion UPGRADES

#pragma region OWN_STRUCTS
typedef struct UnsignedChar2
{
    unsigned char x;
    unsigned char y;
} UnsignedChar2;

typedef struct SignedChar2
{
    signed char x;
    signed char y;
} SignedChar2;
#pragma endregion OWN_STRUCTS

#pragma region PLAYER
typedef struct Player
{
    float x;
    float y;
    short health;
} Player;

typedef enum DIRECTIONS
{
    LEFT = 0,
    RIGHT = 1,
    DOWN = 3,
    UP = 2,
} DIRECTIONS;

typedef struct Inventory
{
    TILE_TYPE type[TILE_COUNT];
    int amount[TILE_COUNT];
} Inventory;

double timer = 0;
#pragma endregion PLAYER

#pragma region ENEMY

#pragma endregion ENEMY

#pragma region MAP
void GenerateOre(Tile map[ROOM_SIZE][ROOM_SIZE], int amount_of_patches, int walks, int steps, TILE_TYPE t)
{
    SignedChar2 directions[4] = {
        (SignedChar2){-1, 0}, // left
        (SignedChar2){1, 0},  // right
        (SignedChar2){0, -1}, // up
        (SignedChar2){0, 1},  // down
    };

    int start_x, point_x;
    int start_y, point_y;

    // p for patches
    for (int p = 0; p < amount_of_patches; p++)
    {
        // Get coordinates of patch
        do
        {
            start_x = point_x = GetRandomValue(0, ROOM_SIZE - 1);
            start_y = point_y = GetRandomValue(25, ROOM_SIZE - 1);
        } while (map[start_x][start_y].hit_points < 0);

        // d for drunkard
        for (int d = 0; d < walks; d++)
        {
            point_x = start_x;
            point_y = start_y;

            // s for steps
            for (int s = 0; s < steps; s++)
            {
                map[point_x][point_y] = tiles_info[t];

                char index = GetRandomValue(0, 3);

                point_x += directions[index].x;
                point_y += directions[index].y;

                if (point_x < 0 || point_x >= ROOM_SIZE)
                    point_x -= directions[index].x;
                if (point_y < 15 || point_y >= ROOM_SIZE)
                    point_y -= directions[index].y;
            }
        }
    }
}

void CarveTunnels(Tile map[ROOM_SIZE][ROOM_SIZE], int passes, char airiness)
{
    char y_offset = 15;
    char chance = airiness;

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
                for (int i = 0; i < 8; i++)
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

    // Having the function for every ore makes it so that I can prioritise what should be on-top
    GenerateOre(map, 150, 10, 3, TILE_FG_COAL);
    GenerateOre(map, 100, 4, 4, TILE_FG_IRON);
}
#pragma endregion MAP

#pragma region POPUP
typedef struct Popup
{
    float timer;
    TILE_TYPE type;
} Popup;

unsigned short pop_up_index = 0;
Popup *popups;

void CreatePopup(TILE_TYPE type)
{
    popups[pop_up_index % MAX_POPUPS] = (Popup){
        .timer = POPUP_TIMER_SECONDS,
        .type = type};
    pop_up_index++;
}
#pragma endregion POPUP

int main()
{
    Player p = (Player){125 * TILE_SIZE, 12 * TILE_SIZE, 100};
    Inventory inventory = {};

    Popup p_arr[MAX_POPUPS];
    popups = p_arr; // malloc(sizeof(Popup) * MAX_POPUPS); // for speed at the moment

    for (int i = 0; i < MAX_POPUPS; i++)
    {
        popups[i].timer = 0;
    }

    {
        // Set random seed
        SetRandomSeed(&p);
    }

    // Create the map and generate the first one
    Tile map[ROOM_SIZE][ROOM_SIZE];
    MapGenerator(map, 1);
    UnsignedChar2 portal_pos = (UnsignedChar2){GetRandomValue(0, ROOM_SIZE - 1), GetRandomValue(ROOM_SIZE - 50, ROOM_SIZE - 1)};
    map[portal_pos.x][portal_pos.y] = tiles_info[TILE_PORTAL];

    // Init the camera
    Camera2D cam = (Camera2D){(Vector2){WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2}, (Vector2){p.x, p.y}, 0, 1};
    bool show_minimap = false;
    bool show_inventory = false;
    bool digging = false;

    // Initialize the window and cap at 500fps - dont need more!
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Game, with procedural!");
    // SetTargetFPS(500);

    // Player hitbox is used in the game-loop
    Rectangle player_hitbox = (Rectangle){p.x - PLAYER_RADIUS * 0.45, p.y - PLAYER_RADIUS * 0.45, TILE_SIZE * 0.9, TILE_SIZE * 0.9};
    Vector2 player_directional_hitboxes[4] = {
        (Vector2){p.x - 5 + TILE_SIZE * 0.9, p.y + PLAYER_RADIUS * 0.45}, // right
        (Vector2){p.x - 8, p.y + PLAYER_RADIUS * 0.45},                   // left
        (Vector2){p.x + PLAYER_RADIUS * 0.45, p.y - 5 + TILE_SIZE * 0.9}, // down
        (Vector2){p.x + PLAYER_RADIUS * 0.45, p.y - 8},                   // up
    };

    DIRECTIONS mining_dir = UP;

    while (!WindowShouldClose())
    {
        // Random key input
        KeyboardKey key = GetKeyPressed();
        if (key != KEY_NULL)
        {
            if (key == KEY_M)
                show_minimap = !show_minimap;
            if (key == KEY_I)
                show_inventory = !show_inventory;
            if (key >= KEY_RIGHT && key <= KEY_UP)
                mining_dir = key - 262;
        }

#pragma region show_inventory
        /*
            START OF SHOW INVENTORY CODE
         */
        if (show_inventory)
        {
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

            // Draw the player
            DrawRectangleRec(player_hitbox, RED);
            // Draw mining pos
            DrawCircle(player_directional_hitboxes[mining_dir].x, player_directional_hitboxes[mining_dir].y, 3, GREEN);

            EndMode2D();

            if (show_minimap)
            {
                Vector2 player_world_to_map_pos = (Vector2){Remap(p.x, 0, ROOM_SIZE * TILE_SIZE, 0, 100), Remap(p.y, 0, ROOM_SIZE * TILE_SIZE, 0, 100)};
                Vector2 portal_world_to_map_pos = (Vector2){Remap(portal_pos.x, 0, ROOM_SIZE - 1, 0, 100), Remap(portal_pos.y, 0, ROOM_SIZE - 1, 0, 100)};
                DrawRectangle(WINDOW_WIDTH - 110, 10, 100, 100, (Color){255, 255, 255, 30});
                DrawCircle(WINDOW_WIDTH - 110 + player_world_to_map_pos.x, 10 + player_world_to_map_pos.y, 5, (Color){255, 0, 0, 100});
                DrawCircle(WINDOW_WIDTH - 110 + portal_world_to_map_pos.x, 10 + portal_world_to_map_pos.y, 5, (Color){255, 0, 255, 100});
            }

            DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){255, 255, 255, 150});

            // Start of inventory
            // Not the nicest, but probably the best code...
            // It cannot be a loop as I have different things I want to display and some I dont
            // For example not showing portal, air and all the background blocks
            // I could do a for loop and do +=2, but oh well

            DrawRectangle(20, 200, WINDOW_WIDTH - 40, WINDOW_HEIGHT - 400, GRAY);
            DrawRectangle(30, 210, 200, WINDOW_HEIGHT - 420, DARKGRAY);

            // -- RESOURCE SECTION --
            // Dirt
            DrawRectangle(35, 220, 180, 25, tile_colors[1]);
            const char *dirt_text = TextFormat("%i dirt", inventory.amount[1]);
            DrawText(dirt_text, 45, 222, 24, BLACK);

            // Stone
            DrawRectangle(35, 255, 180, 25, tile_colors[3]);
            const char *stone_text = TextFormat("%i stone", inventory.amount[3]);
            DrawText(stone_text, 45, 257, 24, BLACK);

            // Iron
            DrawRectangle(35, 290, 180, 25, tile_colors[5]);
            const char *iron_text = TextFormat("%i iron", inventory.amount[5]);
            DrawText(iron_text, 45, 292, 24, BLACK);

            // Coal
            DrawRectangle(35, 325, 180, 25, tile_colors[7]);
            const char *coal_text = TextFormat("%i coal", inventory.amount[7]);
            DrawText(coal_text, 45, 327, 24, WHITE);

            // -- UPGRADE SECTION --
            Rectangle upgrade_secton_info_rec = {WINDOW_HEIGHT / 2 + 100, 220, 210, 360};
            Rectangle upgrade_secton_rec = {WINDOW_HEIGHT / 2 - 100, 210, 420, 380};
            DrawRectangleRec(upgrade_secton_rec, DARKGRAY);

            // Boot upgrade
            Rectangle movement_upgrade_button = {WINDOW_HEIGHT / 2 - 95, 220, 180, 25};
            DrawRectangleRec(movement_upgrade_button, (Color){60, 60, 60, 255});
            const char *movement_text = TextFormat("Upgrade %s", upgrade_names[UPGRADE_SPEED_MOVEMENT]);
            DrawText(movement_text, WINDOW_HEIGHT / 2 - 90, 222, 20, WHITE);

            // Pickaxe upgrade
            Rectangle pickaxe_upgrade_button = {WINDOW_HEIGHT / 2 - 95, 255, 180, 25};
            DrawRectangleRec(pickaxe_upgrade_button, (Color){60, 60, 60, 255});
            const char *mining_text = TextFormat("Upgrade %s", upgrade_names[UPGRADE_SPEED_MINING]);
            DrawText(mining_text, WINDOW_HEIGHT / 2 - 90, 257, 20, WHITE);

            // Display levels by default - will be overwritten by level info
            DrawRectangleRec(upgrade_secton_info_rec, (Color){60, 60, 60, 255});
            const char *pickaxe_upgrade_info_text = TextFormat("Current levels:\nPickaxe: %i\nBoots: %i", mining_level, movement_level);
            DrawText(pickaxe_upgrade_info_text, WINDOW_HEIGHT / 2 + 105, 222, 20, WHITE);

            // Check collisions
            Vector2 mouse_pos = GetMousePosition();
            if (CheckCollisionPointRec(mouse_pos, upgrade_secton_rec))
            {
                // Boot upgrade
                if (CheckCollisionPointRec(mouse_pos, movement_upgrade_button))
                {
                    if (movement_level < LEVELS_COUNT - 1)
                    {
                        // Show req for next level
                        DrawRectangleRec(movement_upgrade_button, (Color){90, 90, 90, 255});
                        DrawText(movement_text, WINDOW_HEIGHT / 2 - 90, 222, 20, WHITE);
                        DrawRectangleRec(upgrade_secton_info_rec, (Color){60, 60, 60, 255});
                        const char *movement_upgrade_info_text = TextFormat("Upgrade to level:%i\nCost:\n- %i %s", movement_level + 1, movement_upgrades_requirements[movement_level + 1].amount, movement_upgrades[movement_level + 1].material_name);
                        DrawText(movement_upgrade_info_text, WINDOW_HEIGHT / 2 + 105, 222, 20, WHITE);
                    }
                    else
                    {
                        // Display it is max level
                        DrawRectangleRec(upgrade_secton_info_rec, (Color){60, 60, 60, 255});
                        const char *movement_upgrade_info_text = TextFormat("Max level!");
                        DrawText(movement_upgrade_info_text, WINDOW_HEIGHT / 2 + 105, 222, 20, WHITE);
                    }
                }

                // Pickaxe upgrade
                else if (CheckCollisionPointRec(mouse_pos, pickaxe_upgrade_button))
                {
                    if (mining_level < LEVELS_COUNT - 1)
                    {
                        // Show req for next level
                        DrawRectangleRec(pickaxe_upgrade_button, (Color){90, 90, 90, 255});
                        DrawText(mining_text, WINDOW_HEIGHT / 2 - 90, 257, 20, WHITE);
                        DrawRectangleRec(upgrade_secton_info_rec, (Color){60, 60, 60, 255});
                        const char *pickaxe_upgrade_info_text = TextFormat("Upgrade to level:%i\nCost:\n- %i %s", mining_level + 1, mining_upgrades_requirements[mining_level + 1].amount, mining_upgrades[mining_level + 1].material_name);
                        DrawText(pickaxe_upgrade_info_text, WINDOW_HEIGHT / 2 + 105, 222, 20, WHITE);
                    }
                    else
                    {
                        // Display it is max level
                        DrawRectangleRec(upgrade_secton_info_rec, (Color){60, 60, 60, 255});
                        const char *pickaxe_upgrade_info_text = TextFormat("Max level!");
                        DrawText(pickaxe_upgrade_info_text, WINDOW_HEIGHT / 2 + 105, 222, 20, WHITE);
                    }
                }

                // Handle buying
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    // Handle boot upgrade
                    if (CheckCollisionPointRec(mouse_pos, movement_upgrade_button))
                    {
                        // Handle buying it
                        if (movement_level < LEVELS_COUNT - 1 && movement_upgrades_requirements[movement_level + 1].amount <= inventory.amount[movement_upgrades_requirements[movement_level + 1].type])
                        {
                            inventory.amount[movement_upgrades_requirements[movement_level + 1].type] -= movement_upgrades_requirements[movement_level + 1].amount;
                            movement_level++;
                        }
                    }

                    // Handle pickaxe upgrade
                    if (CheckCollisionPointRec(mouse_pos, pickaxe_upgrade_button))
                    {
                        // Handle buying it
                        if (mining_level < LEVELS_COUNT - 1 && mining_upgrades_requirements[mining_level + 1].amount <= inventory.amount[mining_upgrades_requirements[mining_level + 1].type])
                        {
                            inventory.amount[mining_upgrades_requirements[mining_level + 1].type] -= mining_upgrades_requirements[mining_level + 1].amount;
                            mining_level++;
                        }
                    }
                }
            }

            // End of displaying inventory
            // Still 500fps, not bad I have to say XD

            // Draw fps
            DrawFPS(10, 10);

            // Draw the time played
            const char *time_played_text = TextFormat("You have played %.2f seconds", timer);
            DrawText(time_played_text, (WINDOW_WIDTH / 2) - (MeasureText(time_played_text, 24) / 2), 100, 24, BLACK);

            EndDrawing();
            continue;
        }

        /*
            END OF SHOW INVENTORY CODE
        */
#pragma endregion show_inventory

        // Add the time to the timer
        timer += GetFrameTime();

        if (IsKeyDown(KEY_SPACE))
        {
            digging = true;
            player_directional_hitboxes[0] = (Vector2){p.x - 5 + TILE_SIZE * 0.9, p.y + PLAYER_RADIUS * 0.45}; // right
            player_directional_hitboxes[1] = (Vector2){p.x - 8, p.y + PLAYER_RADIUS * 0.45};                   // left
            player_directional_hitboxes[2] = (Vector2){p.x + PLAYER_RADIUS * 0.45, p.y - 5 + TILE_SIZE * 0.9}; // down
            player_directional_hitboxes[3] = (Vector2){p.x + PLAYER_RADIUS * 0.45, p.y - 8};                   // up
        }
        else
        {
            digging = false;
        }

        // Input for movement
        Vector2 movement = (Vector2){0, 0};
        float speed = BASE_PLAYER_SPEED * GetFrameTime() * (IsKeyDown(KEY_LEFT_SHIFT) * 1 + 1) * movement_upgrades[movement_level].multiplier;
        movement.x += IsKeyDown(KEY_D) * speed;
        movement.x -= IsKeyDown(KEY_A) * speed;
        movement.y += WORLD_GRAVITY * BASE_PLAYER_SPEED * GetFrameTime(); // IsKeyDown(KEY_S) * speed; // Gravity
        movement.y -= IsKeyDown(KEY_W) * speed * 2;                       // To overpower gravity

        // Check map and character collision
        bool can_move_x = true;
        bool can_move_y = true;
        bool can_move_current = true;
        bool next_level = false;
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

                if (digging)
                {
                    if (CheckCollisionPointRec(player_directional_hitboxes[mining_dir], block))
                    {
                        map[x][y].hit_points -= mining_upgrades[mining_level].multiplier * GetFrameTime();
                        if (map[x][y].hit_points <= 0)
                        {
                            inventory.amount[map[x][y].type]++;
                            CreatePopup(map[x][y].type);
                            map[x][y].type += 1;
                        }
                    }
                }
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

#pragma region game_drawing
        // Start the drawing loop
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

        // Draw the player
        DrawRectangleRec(player_hitbox, RED);
        // Draw mining pos
        DrawCircle(player_directional_hitboxes[mining_dir].x, player_directional_hitboxes[mining_dir].y, 3, GREEN);

        EndMode2D();

        if (show_minimap)
        {
            Vector2 player_world_to_map_pos = (Vector2){Remap(p.x, 0, ROOM_SIZE * TILE_SIZE, 0, 100), Remap(p.y, 0, ROOM_SIZE * TILE_SIZE, 0, 100)};
            Vector2 portal_world_to_map_pos = (Vector2){Remap(portal_pos.x, 0, ROOM_SIZE - 1, 0, 100), Remap(portal_pos.y, 0, ROOM_SIZE - 1, 0, 100)};
            DrawRectangle(WINDOW_WIDTH - 110, 10, 100, 100, (Color){255, 255, 255, 30});
            DrawCircle(WINDOW_WIDTH - 110 + player_world_to_map_pos.x, 10 + player_world_to_map_pos.y, 5, (Color){255, 0, 0, 100});
            DrawCircle(WINDOW_WIDTH - 110 + portal_world_to_map_pos.x, 10 + portal_world_to_map_pos.y, 5, (Color){255, 0, 255, 100});
        }

        short offset_counter_y = 0; // Can be a counter that multiplies with a constant
        for (int i = MAX_POPUPS - 1; i >= 0; i--)
        {
            if (popups[i].timer > 0)
            {
                Color c = (Color){0, 0, 0, Remap(popups[i].timer, POPUP_TIMER_SECONDS, 0, 200, 0) + 55}; // this can be done better with alpha color
                DrawText(TextFormat("Mined 1 %s", block_names[popups[i].type]), 10, WINDOW_HEIGHT - 28 - offset_counter_y, 24, c);
                popups[i].timer -= GetFrameTime();
                offset_counter_y += 18;

                if (popups[i].timer <= 0)
                {
                    pop_up_index--;
                }
            }
        }

        // Always the fps counter
        DrawFPS(10, 10);
        EndDrawing();
#pragma endregion game_drawing

        if (CheckCollisionRecs(player_hitbox, (Rectangle){portal_pos.x * TILE_SIZE, portal_pos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE}))
        {
            MapGenerator(map, 1);
            p.x = (TILE_SIZE * ROOM_SIZE) / 2;
            p.y = 10 * TILE_SIZE;
            portal_pos = (UnsignedChar2){GetRandomValue(0, ROOM_SIZE - 1), GetRandomValue(ROOM_SIZE - 50, ROOM_SIZE - 1)};
            map[portal_pos.x][portal_pos.y] = tiles_info[TILE_PORTAL];
            printf("Generated new map!\n");
        }
    }

    return 0;
}