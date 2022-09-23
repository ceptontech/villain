/**
 * @file villain-viewer.cpp
 * @author Michael J Vernau (michael.vernau@cepton.com)
 * @brief 
 * @version 0.3
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
#define BKG_FILE_LOCATION "../data/bkg_0.csv"
#define TRACKING_FILE_LOCATION "../npz/tracking.csv"
//point data params
#define NUM_FRAMES 500
#define BKG_THRESHOLD 0.5
#define EXPECTED_POINTS_PER_FRAME 60000
#define DPP 3 //datapoints per point(x,y,z)
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
    std::cout << "[info] starting cluster loading\n";
    std::fstream tk_file;
    tk_file.open(TRACKING_FILE_LOCATION);
    std::vector<std::vector<std::vector<float>>> clusters;
    clusters.reserve(NUM_FRAMES);
    if(tk_file.is_open()){
        std::string s;
        std::getline(tk_file, s);
        int curr_frame = 1;
        std::vector<std::vector<float>> frame;
        frame.reserve(MAX_CLUSTERS);
        while(std::getline(tk_file, s)){
            std::vector<float> cluster;
            cluster.reserve(CLUSTER_FIELDS);
            auto start = 0U;
            auto end = s.find(delim);
            int place_idx = 0;
            int row_idx = 0;
            float clstr_data[CLUSTER_FIELDS];
            while(end != std::string::npos){
                cluster.push_back(stof(s.substr(start, end - start)));
                start = end + delim.length();
                end = s.find(delim, start);
            } 
            if((int)cluster[0] > curr_frame){
                clusters.push_back(frame);
                frame = std::vector<std::vector<float>>();
                frame.reserve(MAX_CLUSTERS);
                curr_frame++;
            }
            frame.push_back(cluster);
        }
    }
    std::cout << "[info] cluster array loaded\n";
    std::cout.flush();
    tk_file.close();
    std::unordered_map<int, float> bkg_probs;
    std::fstream bkg_file;
    bkg_file.open(BKG_FILE_LOCATION, std::ios::in);
    std::cout << "[info] starting bkg load\n";
    if (bkg_file.is_open()){
        std::string s;
        while(std::getline(bkg_file, s)){
            auto start = 0U;
            auto end = s.find(delim);
            int bkt[3];
            int idx = 0;
            while(end != std::string::npos){
                bkt[idx] = (int)std::stof(s.substr(start, end - start));
                start = end + delim.length();
                end = s.find(delim, start);
                idx++;
            }
            float val = std::stof(s.substr(start, end));
            if(val >= BKG_THRESHOLD){
                bkg_probs[flattened(bkt[0], bkt[1], bkt[2])] = val;
            }
        }
    }
    bkg_file.close();
    std::cout << "[info] bkg size is: " << bkg_probs.size() << std::endl;
    std::cout << "[info] bkg loaded; starting frame load\n";
    std::vector<std::vector<std::vector<float>>> frames; //frames holds all frames data
    frames.reserve(NUM_FRAMES);
    for(int i=0; i < NUM_FRAMES; i++){ // load data from csv to triple vector of ints
        std::string frame_path = FRAME_PATH;
        frame_path = frame_path + std::to_string(i) + std::string(".csv");
        std::vector<std::vector<float>> pts;
        pts.reserve(EXPECTED_POINTS_PER_FRAME);
        std::fstream file;
        file.open(frame_path, std::ios::in);
        if(file.is_open()){
            std::string s;
            while(std::getline(file,s)){
                auto start = 0U;
                auto end = s.find(delim);
                std::vector<float> pt;
                pt.reserve(DPP);
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
        if(i% 50 == 0){
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
    int nq = 0;
    while (!WindowShouldClose())
    {
        UpdateCamera(&camera);
        BeginDrawing();
        ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                DrawGrid(10, 5.0f);
                for (auto& clst : clusters[n]){
                    float x_len = clst[10] - clst[9];
                    float y_len = clst[12] - clst[11];
                    float z_len = clst[14] - clst[13];
                    float x_mean = clst[10] + clst[9] / 2.0f;
                    float y_mean = clst[12] + clst[11] / 2.0f;
                    float z_mean = clst[14] + clst[13] / 2.0f;
                    Vector3 cube_pos = {x_len, z_len, y_len};
                    Color cube_color = RED;
                    DrawCubeWires(cube_pos, x_len, z_len, y_len, cube_color);
                }
                for(std::vector<float> pt : frames[n]){
                    Vector3 xyz = {pt[0], pt[2], pt[1]};
                    Color pt_color = ColorFromHSV((float)((int)(pt[4] * 600.0)%360), 0.75f, 0.9f);
                    if((bkg_probs.find(vox_assignment(pt[0], pt[1], pt[2])) != bkg_probs.end()) || (pt[2] < 0.05)){
                        pt_color = GRAY;
                        DrawPoint3D(xyz, pt_color);
                    }else{
                        bool drawn = false;
                        for(auto& clst : clusters[n]){
                            if((pt[0] > clst[9])&(pt[0] < clst[10])&&(pt[1] > clst[11])&(pt[1] < clst[12])&&(pt[2] > clst[13])&(pt[2] < clst[14])){
                                pt_color = ColorFromHSV((float)((int)clst[1]%NUM_CLUSTER_COLORS)/NUM_CLUSTER_COLORS * 360.0f, 0.75f, 0.9f);
                                DrawPoint3D(xyz, pt_color);
                                drawn = true;
                            }
                        }
                        if(!drawn){
                            pt_color = BLACK;
                            DrawPoint3D(xyz, pt_color);
                        }
                    }
                }
               
            EndMode3D();
            DrawFPS(10, 10);
        EndDrawing();
        nq++;
        nq %= 3;
        if (nq == 0){
            n++;
        }
        if(n >= NUM_FRAMES){
            n = 0;
        }

    }
    CloseWindow();        // Close window and OpenGL context
    return 0;
}
