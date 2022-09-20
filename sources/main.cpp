#include "raylib.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define SCREEN_WIDTH (1600)
#define SCREEN_HEIGHT (900)
#define FRAME_PATH "../data/frame_"
#define NUM_FRAMES 5693
#define WINDOW_TITLE "Cepton Viewer (Raylib)"

int main()
{
    std::vector<std::vector<std::vector<float>>> frames; //frames holds all frames data
    for(int i=0; i < NUM_FRAMES; i++){ // load data from csv to triple vector of ints
        std::string frame_path = FRAME_PATH;
        frame_path = frame_path + std::to_string(i) + std::string(".csv");
        std::vector<std::vector<float>> pts;
        std::fstream file;
        file.open(frame_path, std::ios::in);
        std::string delim = ",";
        if(file.is_open()){
            std::string s;
            while(std::getline(file,s)){
                auto start = 0U;
                auto end = s.find(delim);
                std::vector<float> pt;
                while(end != std::string::npos){
                    pt.push_back(std::stof(s.substr(start, end - start)));
                    start = end + delim.length();
                    end = s.find(delim, start);
                }
                pt.push_back(std::stof(s.substr(start, end)));
                pts.push_back(pt);
            }
        }
        frames.push_back(pts);
        file.close();
        if(i% 500 == 0){
            std::cout << "[info] frame " << i << " loaded\n";
        }
    }
    std::cout << "[info] frames loaded, starting visualization\n";
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE); //start rendering window
    // Initialize the camera
    Camera3D camera = { { 10.0f, 1.4f, 0.0f }, { 16.0f, 1.4f, -16.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 }; // {posistion, look posistion, axis, FOV, ???}
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    SetTargetFPS(40);
    int n = 0;
    while (!WindowShouldClose())
    {
        UpdateCamera(&camera);
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                DrawGrid(10, 5.0f);
                for(std::vector<float> pt : frames[n]){
                    Vector3 xyz = {pt[0], pt[2], pt[1]};
                    Color pt_color = ColorFromHSV((float)((int)(pt[4] * 600.0)%360), 0.75f, 0.9f);
                    DrawPoint3D(xyz, pt_color);
                }
            EndMode3D();
            DrawFPS(10, 10);
        EndDrawing();
        n++;
        if(n >= NUM_FRAMES){
            n = 0;
        }

    }
    CloseWindow();        // Close window and OpenGL context
    return 0;
}
