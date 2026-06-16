#include <raylib.h>

int main()
{
    InitWindow(800, 600, "Raylib Test");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        
        ClearBackground(BLACK);
        DrawText("Raylib Works!", 300, 280, 30, WHITE);
        DrawCircle(600, 300, 40, GREEN);
        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();
}