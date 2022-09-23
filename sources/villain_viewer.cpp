/**
 * @file villain-viewer.cpp
 * @author Michael J Vernau (michael.vernau@cepton.com)
 * @brief 
 * @version 0.4
 * @date 2022-09-21
 * 
 */
#include "raylib.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

//raylib params
#define SCREEN_WIDTH (1600)
#define SCREEN_HEIGHT (900)
#define WINDOW_TITLE "Cepton Villain Viewer"
#define NUM_CLUSTER_COLORS 15
//file params
#define FRAME_PATH "../data/frame_"
#define BKG_FILE_LOCATION "../data/bkg.csv"
#define TRACKING_FILE_LOCATION "../npz/tracking.csv"
//point data params
#define NUM_FRAMES 500
#define BKG_THRESHOLD 0.5
#define EXPECTED_POINTS_PER_FRAME 60000
#define DPP 10 //datapoints per point(x,y,z)
//voxel params
#define X_DIM 310
#define Y_DIM 510
#define Z_DIM 70
#define dx 0.05
#define dy 0.05
#define dz 0.05
//cluster params
#define MAX_CLUSTERS 100
#define CLUSTER_FIELDS 10


/**
 * @brief flattens our 3d voxel space into one D
 * 
 * @param x int ix or x bin
 * @param y int iy or y bin
 * @param z int iz or z bin
 * @return int 1d bin index
 */
int flattened(int x, int y, int z){
    return x + (y * Y_DIM) + (z * X_DIM * Y_DIM);
}
void unbox(std::vector<float>& ptdata){
    ptdata[0] *= dx;
    ptdata[0] += dx/2.0f;
    ptdata[1] *= dy;
    ptdata[1] += dy/2.0f;
    ptdata[2] *= dz;
    ptdata[2] += dz/2.0f;
}

/**
 * @brief assigns a 1d bin from a 3d coordinate 
 * 
 * @param x float x coord 
 * @param y float y coord
 * @param z float z coord
 * @return int 1d bin index assignment
 */
int vox_assignment(float x, float y, float z){
    return flattened((int)x/dx, (int)y/dy, (int)z/dz);
}
/**
 * @brief TODO: not decomped at all
 * 
 * @return int 
 */
int main()
{
    std::string delim = ",";
    std::cout << "[INFO] bkg point load beginning\n";
    std::vector<std::vector<float>> bkg;
    std::fstream bkg_file;
    bkg_file.open(BKG_FILE_LOCATION);
    if(bkg_file.is_open()){
        std::string line;
        std::getline(bkg_file, line);
        while(std::getline(bkg_file, line)){
            auto start = 0U;
            auto end = line.find(delim);
            std::vector<float> bkg_point;
            while(end != std::string::npos){
                bkg_point.push_back(std::stof(line.substr(start, end - start)));
                start = end + delim.length();
                end = line.find(delim, start);
            }
            bkg_point.push_back(std::stof(line.substr(start, end)));
            if(bkg_point[3] > BKG_THRESHOLD){
                    unbox(bkg_point);
                    bkg.push_back(bkg_point);
            }
        }
    }else{
        std::cerr << "bkg file failed to open\n";
        return 1;
    }
    bkg_file.close();
    std::cout << "[INFO] bkg loaded " << bkg.size() << " pts\n[INFO] beginning foreground point load\n";
    std::vector<std::vector<std::vector<float>>> frames;
    frames.reserve(NUM_FRAMES);
    std::string frame_path = std::string(FRAME_PATH);
    for(int frame_number = 0; frame_number < NUM_FRAMES; frame_number++){
        std::vector<std::vector<float>> frame;
        frame.reserve(EXPECTED_POINTS_PER_FRAME);
        std::fstream frame_file;
        frame_file.open((frame_path + std::to_string(frame_number) + std::string(".csv")));
        if(frame_file.is_open()){
            std::string line;
            std::getline(frame_file, line);
            while(std::getline(frame_file, line)){
                auto start = 0U;
                auto end = line.find(delim);
                std::vector<float> pt;
                pt.reserve(DPP);
                while(end != std::string::npos){
                    pt.push_back(std::stof(line.substr(start, end - start)));
                    start = end + delim.length();
                    end = line.find(delim, start);
                }
                pt.push_back(std::stof(line.substr(start, end)));
                frame.push_back(pt);
            }
            frames.push_back(frame);
            frame_file.close();
        if(frame_number%100 == 0){
            std::cout << "[INFO] frame loading " << (float)((float)frame_number/NUM_FRAMES * 100.0f) << "\% complete\n";
        }
        }else{
            std::cout << "[WARNING] frame " << frame_number << "failed to open \n";
        }
    }
    std::cout << "[INFO] frames locked and loaded\n";
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE); //start rendering window
    // Initialize the camera
    Camera3D camera = { { 10.0f, 1.4f, 0.0f }, { 16.0f, 1.4f, -16.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 }; // {posistion, look posistion, axis, FOV, ???}
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    SetTargetFPS(60);
    int n = 0;
    while(!WindowShouldClose()){
        UpdateCamera(&camera);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        DrawGrid(10, 5.0f);
        for(auto& bpt : bkg){
            Vector3 bpos = {bpt[0], bpt[2], bpt[1]};
            Color bcolor = BLACK;
            DrawPoint3D(bpos, bcolor);
        }
        for(auto& pt : frames[n]){
            int sn = (int)pt[7];
            if (sn >= 0){
                Vector3 xyz = {pt[0], pt[2], pt[1]};
                Color pt_color = ColorFromHSV((((float)(sn%NUM_CLUSTER_COLORS)) / NUM_CLUSTER_COLORS) * 360.0f, 0.75f, 0.9f);
                DrawPoint3D(xyz, pt_color);
            }
        }
        EndMode3D();
        DrawFPS(10,10);
        EndDrawing();
        n++;
        if(n >= NUM_FRAMES){
            n = 0;
        }
    }
    return 0;
}
