#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

// This makes a nice galaxy!

#define WINDOW_WIDTH (short)800
#define WINDOW_HEIGHT (short)800
#define AMOUNT_OF_RINGS (int)100
#define AMOUNT_OF_DROPS (int)200

int main()
{
    Vector2 points[AMOUNT_OF_DROPS];

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Round Noice");

    Image gr = GenImageGradientRadial(WINDOW_WIDTH, WINDOW_HEIGHT, 0.5, YELLOW, PURPLE);
    Texture2D gr_texture = LoadTextureFromImage(gr);
    UnloadImage(gr);

    for (int i = 0; i < AMOUNT_OF_DROPS; i++)
    {
        points[i] = (Vector2){GetRandomValue(0, WINDOW_WIDTH), GetRandomValue(0, WINDOW_HEIGHT)};
    }

    int gradient = floor(255 / AMOUNT_OF_RINGS);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        DrawTexture(gr_texture, 0, 0, WHITE);

        for (int i = 0; i < AMOUNT_OF_DROPS; i++)
        {
            for (int s = 1; s <= AMOUNT_OF_RINGS; s++)
            {
                float radius = 100 / s;
                DrawCircle(points[i].x, points[i].y, radius, (Color){gradient * s, gradient * s, gradient * s, 150});
            }
        }

        EndDrawing();
    }

    return 0;
}