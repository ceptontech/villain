#include "raylib.h"
#include <math.h>

#define SCREEN_WIDTH (1600)
#define SCREEN_HEIGHT (900)
#define NUM_BLOCKS (15)

#define WINDOW_TITLE "Cepton Viewer (Raylib)"

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    // Initialize the camera
    Camera3D camera = { { 20.0f, 0.4f, 40.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        double time = GetTime();
        UpdateCamera(&camera);
        float scale = (2.0f + (float)sin(time))*0.7f;
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                DrawGrid(10, 5.0f);
                for (int x = 0; x < NUM_BLOCKS; x++)
                {
                    for (int y = 0; y < NUM_BLOCKS; y++)
                    {
                        for (int z = 0; z < NUM_BLOCKS; z++)
                        {
                            float blockScale = (x + y + z)/30.0f;
                            float scatter = sinf(blockScale*20.0f + (float)(time*4.0f));

                            Vector3 cubePos = {
                                (float)(x - NUM_BLOCKS/2)*(scale*3.0f) + scatter,
                                (float)(y - NUM_BLOCKS/2)*(scale*2.0f) + scatter,
                                (float)(z - NUM_BLOCKS/2)*(scale*3.0f) + scatter
                            };
                            Color cubeColor = ColorFromHSV((float)(((x + y + z)*18)%360), 0.75f, 0.9f);
                            float cubeSize = (2.4f - scale)*blockScale;
                            DrawCube(cubePos, cubeSize, cubeSize, cubeSize, cubeColor);
                        }
                    }
                }
            EndMode3D();
            DrawFPS(10, 10);
        EndDrawing();

    }
    CloseWindow();        // Close window and OpenGL context
    return 0;
}
