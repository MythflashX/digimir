#include <iostream>
#include <raylib.h>
#include <fstream>
#include <string>
#include <array>

int safe_conversion(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return 0;
    }
}

std::array<int, 5> import_instructions()
{
    std::ifstream file {"convergence.txt"};
    std::string data;
    std::array<int, 5> instructions;
    std::string temp {""};
    unsigned int index {4};
    if (!file) {
        std::cerr << "Unable to read the file";
        return {0, 0, 0, 0, 0};
    }

    std::getline(file, data);
    for (char element : data) {
        if (element == ',') {
            instructions[index] = safe_conversion(temp);           // convert the number from string to int and add to the instructions
            index--;    temp = "";                      // reset temporary data holder
        }
        else {
            temp.push_back(element);
        }
    }
    instructions[0] = safe_conversion(temp);   // add the last value because iterator ended befor it could add the final value

    return instructions;
}

std::array<int, 5> frame_converter(std::array<int, 5> instructions)
{
    std::array<int, 5> frames_sync;
    for (unsigned int index{0}; index < 5; index++) 
        frames_sync[index] = int(instructions[index] * (60.0 / 100.0));       // convert the percentage from instruction to value between 0-60 since we run 60 frame per second
    for (int element : frames_sync)
        std::cout << element << '|';

    return frames_sync;
}

void control(const std::array<int, 5> &frames_sync, std::array<int, 5> &frames)
{
    for (unsigned int index {0}; index < 5; index++) {
        if (frames_sync[index] != frames[index]) {
            if (frames_sync[index] > frames[index])
                frames[index]++;
            else
                frames[index]--;
        }
    }
}

int main ()
{
/* - se[t]up - */
    // __display__
    constexpr unsigned int screen_width {900};
    constexpr unsigned int screen_height {500};
    InitWindow(screen_width, screen_height, "Simulation");
    // __camera__
    Camera camera {0};
    camera.position = {8, 5, 10};
    camera.target = {0, 0, 0};
    camera.up  = {0, 0.5, 0};
    camera.fovy = 80;
    // __model__
    Vector3 model_pos []{       // [fingers]
        {0, 0, -4},             // pinky  _
        {0, 0, -2},             // ring   _|
        {0, 0, 0},              // middle _|
        {0, 0, 2},              // index  _|-[positions]
        {0, -2, 3},             // thumb  _| 
        {0, -3, -1}             // palm   _|                (unused probably will in later time)
    }; 
    const char* model_paths[] {
        "digitus.glb",       //  _
        "digitus.glb",       //  _|
        "digitus.glb",       //  _|
        "digitus.glb",       //  _|-[paths]
        "pollex.glb",        //  _|
        "palm.glb"           //  _|
    };
    // __animation__
    unsigned int animation_count {0};
    int animation_id {0};
    std::array<int, 5> frames {0, 0, 0, 0, 0};


/* - lo[a]d - */
    // __model__
    Model models[5] {0};
    ModelAnimation *anims[5] {0};
    for (int i {0}; i < 5; i++) {
        models[i] = LoadModel(model_paths[i]);          // Load each model from paths specified and store them in models array
        anims[i] = LoadModelAnimations(model_paths[i], &animation_count); // same as above but this time for animation
    }


/* - simu[l]ation - */
    DisableCursor();
    SetTargetFPS(60);
    while(!WindowShouldClose()) {

/* - update - */
        // __camera__
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        // __instructions__
        std::array<int, 5> frames_sync = frame_converter(import_instructions());
        // __model__
        control(frames_sync, frames);
        for (int i{0}; i < 5; i++)
            UpdateModelAnimation(models[i], anims[i][animation_id], frames[i]);         // update model's animation based on key input
/* - dis[p]lay - */
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                DrawModel(models[0], model_pos[0], 2.0f, GREEN);
                DrawModel(models[1], model_pos[1], 2.0f, PINK);
                DrawModel(models[2], model_pos[2], 2.0f, YELLOW);
                DrawModel(models[3], model_pos[3], 2.0f, BLUE);
                DrawModel(models[4], model_pos[4], 2.0f, RED);
                DrawCube(model_pos[5], 1, 6, 9, BLACK);                 // forgot to fix the palm model...
            EndMode3D();
        EndDrawing();
    }


/* - dis[m]antle - */
    CloseWindow();
    return 0;
}