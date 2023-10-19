#include "raylib.h"
#include "raymath.h"

#define WINDOW_WIDTH (short)800
#define WINDOW_HEIGHT (short)800

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Image testing");

    Image cells = GenImageCellular(200, 200, 50);
    Image checked = GenImageChecked(200, 200, 10, 10, RED, BLACK);
    Image white = GenImageWhiteNoise(200, 200, 0.5);
    Image gh = GenImageGradientH(200, 200, RED, BLUE);
    Image gr = GenImageGradientRadial(200, 200, 0.5, RED, BLUE);
    Image gv = GenImageGradientV(200, 200, YELLOW, RED);

    Texture2D cells_texture = LoadTextureFromImage(cells);
    Texture2D checked_texture = LoadTextureFromImage(checked);
    Texture2D white_texture = LoadTextureFromImage(white);
    Texture2D gh_texture = LoadTextureFromImage(gh);
    Texture2D gr_texture = LoadTextureFromImage(gr);
    Texture2D gv_texture = LoadTextureFromImage(gv);

    UnloadImage(cells);
    UnloadImage(checked);
    UnloadImage(white);
    UnloadImage(gh);
    UnloadImage(gr);
    UnloadImage(gv);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GRAY);

        DrawTexture(cells_texture, 0, 0, WHITE);
        DrawTexture(checked_texture, 200, 0, WHITE);
        DrawTexture(white_texture, 400, 0, WHITE);
        DrawTexture(gh_texture, 600, 0, WHITE);
        DrawTexture(gr_texture, 0, 200, WHITE);
        DrawTexture(gv_texture, 200, 200, WHITE);

        EndDrawing();
    }

    return 0;
}