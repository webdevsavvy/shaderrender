/*******************************************************************************************
 *
 *   raylib [shaders] example - Julia sets
 *
 *   Example complexity rating: [★★★☆] 3/4
 *
 *   NOTE: This example requires raylib OpenGL 3.3 or ES2 versions for shaders support,
 *         OpenGL 1.1 does not support shaders, recompile raylib to OpenGL 3.3 version.
 *
 *   NOTE: Shaders used in this example are #version 330 (OpenGL 3.3).
 *
 *   Example originally created with raylib 2.5, last time updated with raylib 4.0
 *
 *   Example contributed by Josh Colclough (@joshcol9232) and reviewed by Ramon Santamaria (@raysan5)
 *
 *   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
 *   BSD-like license that allows static linking with closed source software
 *
 *   Copyright (c) 2019-2025 Josh Colclough (@joshcol9232) and Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <raylib.h>

#include "ffmpeg.h"

#define GLSL_VERSION 330

#define READ_END 0
#define WRITE_END 1

#define STR_EQ(__s1, __s2) (strcmp(__s1, __s2) == 0)

// Global variable declarations
//------------------------------------------------------------------------------------------

size_t screenWidth = 1200; // screen width in pixels
size_t screenHeight = 800; // screen height in pixels
size_t fps = 60; // number of frames per second

char *outputModeArg = "window"; // argument for the type of rendering output (default `window`)
char *fragShaderArg; // argument for the relative path to fragment shader file
char *vertShaderArg; // argument for the relative path to vertex shader file
int durationArg = 1; // argument for the duration of video output

bool fragShaderFlag = false; // flag set if fragment shader argument is passed
bool vertShaderFlag = false; // flag set if vertex shader argument is passed
bool durationFlag = false; // flag set if duration of video output argument is passed

// Function declarations
//------------------------------------------------------------------------------------------

/**
 * @brief Parse the CLI arguments and set the global flags and arguments.
 * 
 * @param argc 
 * @param argv 
 */
void ParseProgramArguments(int argc, char **argv);

/**
 * @brief Run the windowed pipeline with raylib.
 *
 * @param windowTitle
 */
void RunWindowedRenderer(const char *windowTitle);

/**
 * @brief Run with a pipe to ffmpeg to record as video.
 *
 * @param windowTitle
 */
void RunVideoRenderer(const char *windowTitle);

/**
 * @brief Print the usage for this program.
 * 
 */
void PrintProgramUsage(char *programName);

/**
 * @brief Load shader from files and set resolution uniform
 *
 * @param vertPath Path to vertex shader or NULL
 * @param fragPath Path to fragment shader or NULL
 * @return Shader The loaded shader
 */
Shader LoadShaderAndSetResolution(const char *vertPath, const char *fragPath);

/**
 * @brief Program main entry point.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char **argv)
{
    ParseProgramArguments(argc, argv);

    if (strcmp(outputModeArg, "window") == 0) {
        printf("[shader-renderer] Running Windowed Renderer with options: fragment shader %s\n", fragShaderArg);
        RunWindowedRenderer("shader-renderer");
        return 0;
    }

    if (strcmp(outputModeArg, "video") == 0) {
        printf("[shader-renderer] Running Video Renderer with options: fragment shader %s\n", fragShaderArg);
        RunVideoRenderer("shader-renderer");
        return 0;
    }

    return 1;
}

void PrintProgramUsage(char *programName) 
{
    printf("Usage: %s [-f <fragment_shader>] [-v <vertex_shader>] [-m <output_mode>] [-d <duration>]\n", programName);
    printf("\t|Argument|\t\t\t|Description|\n");
    printf("\t<fragment_shader>\t\tRelative path to the fragment shader code file\n");
    printf("\t<vertex_shader>\t\t\tRelative path to the vertex shader code file\n");
    printf("\t<output_mode>\t\t\tOutput mode for the shader rendering (window | video, default: window)\n");
    printf("\t<duration>\t\t\tDuration of video output in seconds\n");
}

void ParseProgramArguments(int argc, char **argv)
{
    if (argc == 1)
    {
        PrintProgramUsage(argv[0]);
        exit(0);
    }

    for (int i = 0; i < argc; i++)
    {
        char *arg = argv[i];

        if (STR_EQ(arg, "-m"))
        {
            outputModeArg = argv[i + 1];
            printf("[shader-renderer] Setting enabled: output mode %s\n", outputModeArg);
            continue;
        }

        if (strcmp(arg, "-f") == 0)
        {
            fragShaderArg = argv[i + 1];
            fragShaderFlag = true;
            printf("[shader-renderer] Setting enabled: fragment shader %s\n", argv[i + 1]);
            continue;
        }

        if (strcmp(arg, "-d") == 0)
        {
            durationArg = atoi(argv[i + 1]);
            durationFlag = true;
            printf("[shader-renderer] Setting enabled: duration %d\n", durationArg);
            continue;
        }
    }
}

void RunWindowedRenderer(const char *windowTitle)
{
    SetTraceLogLevel(LOG_INFO);

    SetConfigFlags(FLAG_WINDOW_UNDECORATED);

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, TextFormat("%s", windowTitle));

    SetTargetFPS(fps);

    Shader shader = LoadShaderAndSetResolution(
        vertShaderFlag ? vertShaderArg : 0,
        fragShaderFlag ? fragShaderArg : 0
    );

    float time = 0.0f;
    int timeLoc = GetShaderLocation(shader, "u_time");

    // Main game loop
    //--------------------------------------------------------------------------------------

    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        time += GetFrameTime();

        if (IsKeyPressed(KEY_R))
        {
            printf("[shader-renderer] Reloading shader ...\n");

            UnloadShader(shader);

            shader = LoadShaderAndSetResolution(
                vertShaderFlag ? vertShaderArg : 0,
                fragShaderFlag ? fragShaderArg : 0
            );

            // Update time location (might have changed in the new shader)
            timeLoc = GetShaderLocation(shader, "u_time");

            printf("[shader-renderer] Shader reloaded successfully\n");
        }

        SetShaderValue(shader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
        BeginDrawing();
        ClearBackground(WHITE); // Clear screen background

        BeginShaderMode(shader);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
        EndShaderMode();
        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(shader);        // Unload shader

    // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    CloseWindow();
}

void RunVideoRenderer(const char *windowTitle) 
{
    // int ffmpeg = ffmpeg_start_rendering(screenWidth, screenHeight, fps);
    FFMPEG *ffmpeg = InitFFMPEG(screenWidth, screenHeight, fps);

    SetTraceLogLevel(LOG_INFO);

    SetConfigFlags(FLAG_WINDOW_UNDECORATED);

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, TextFormat("%s", windowTitle));

    SetTargetFPS(fps);

    Shader shader = LoadShader(vertShaderFlag ? vertShaderArg : 0, fragShaderFlag ? fragShaderArg : 0);

    // Set Shader Uniform for screenspace resolution
    float resolution[2] = {(float)screenWidth, (float)screenHeight};
    SetShaderValue(shader, GetShaderLocation(shader, "u_resolution"), resolution, SHADER_UNIFORM_VEC2);

    float time = 0;
    int timeLoc = GetShaderLocation(shader, "u_time");

    // Main game loop
    //--------------------------------------------------------------------------------------

    for (int frame = 0; frame < fps * durationArg && !WindowShouldClose(); frame++) // Run only for the necesary frames
    {
        // Update
        //----------------------------------------------------------------------------------
        time += GetFrameTime();

        SetShaderValue(shader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
        BeginDrawing();
        ClearBackground(WHITE); // Clear screen background
        

        BeginShaderMode(shader);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
        EndShaderMode();
        
        EndDrawing();
        
        Image image = LoadImageFromScreen();
        SendFrameFFMPEG(ffmpeg, image.data, screenWidth, screenHeight);
        UnloadImage(image);
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(shader);        // Unload shader
    UnloadFFMPEG(ffmpeg);

    // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    CloseWindow();
}

Shader LoadShaderAndSetResolution(const char *vertPath, const char *fragPath)
{
    Shader shader = LoadShader(vertPath, fragPath);

    // Set Shader Uniform for screenspace resolution
    float resolution[2] = {(float)screenWidth, (float)screenHeight};
    SetShaderValue(shader, GetShaderLocation(shader, "u_resolution"), resolution, SHADER_UNIFORM_VEC2);

    return shader;
}
