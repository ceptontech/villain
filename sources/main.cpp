#include "raylib.h"

#define SCREEN_WIDTH (1600)
#define SCREEN_HEIGHT (900)

#define WINDOW_TITLE "Cepton Viewer (Raylib)"

int main(void)
{
    InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);
    Texture2D logo = LoadTexture(ASSETS_PATH"cepton-logo.png");
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(logo, SCREEN_WIDTH/2 - logo.width/2, SCREEN_HEIGHT/2 - logo.width/2, RED);
        char* txt = "CEPTON EVIL INC";
        Vector2 txtsize = MeasureTextEx(GetFontDefault(),txt,40,1);
        DrawText(txt, SCREEN_WIDTH/2 - txtsize.x/2, SCREEN_HEIGHT*3/4,40, BLACK);
        EndDrawing();
    }
    return 0;
}
