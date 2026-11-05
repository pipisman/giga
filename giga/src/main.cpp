#include "raylib.h"

int main()
{
    InitWindow(800, 450, "window");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("test", 280, 190, 32, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
