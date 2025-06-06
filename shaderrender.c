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

#define streq(__s1, __s2) (strcmp(__s1, __s2) == 0)

// Global variable declarations
//------------------------------------------------------------------------------------------

size_t screenWidth = 1200; // screen width in pixels
size_t screenHeight = 800; // screen height in pixels
size_t fps = 60;           // number of frames per second

char *outputModeArg = NULL; // argument for the type of rendering output (default `window`)
char *fragShaderArg;        // argument for the relative path to fragment shader file
char *vertShaderArg;        // argument for the relative path to vertex shader file
int durationArg = 1;        // argument for the duration of video output

bool fragShaderFlag = false; // flag set if fragment shader argument is passed
bool vertShaderFlag = false; // flag set if vertex shader argument is passed
bool durationFlag = false;   // flag set if duration of video output argument is passed

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

    if (outputModeArg == NULL)
    {
        printf("ERROR: The argument -m <output-mode> was not provided!\n");
        PrintProgramUsage(argv[0]);
        exit(1);
    }

    if (!fragShaderFlag && ! vertShaderFlag) {
        printf("ERROR: The arguments -f <fragment-shader> and -v <vertex-shader> are missing!\nAt least one of them must be set.\n");
        PrintProgramUsage(argv[0]);
        exit(1);
    }

    if (streq(outputModeArg, "window"))
    {

        RunWindowedRenderer("shaderrender");
        exit(0);
    } 
    
    else if (streq(outputModeArg, "video"))
    {
        RunVideoRenderer("shaderrender");
        exit(0);
    }

    else
    {
        printf("ERROR: The argument -m <output-mode> was not recognized!\n'%s' is not a valid output mode, please use one of these ('window', 'video')\n", outputModeArg);
        PrintProgramUsage(argv[0]);
        exit(1);
    }
}

void PrintProgramUsage(char *programName)
{
    printf("Usage: %s [-f <fragment-shader>] [-v <vertex-shader>] [-m <output-mode>] [-d <duration>]\n", programName);
    printf("\t-f <fragment-shader>\t\tRelative path to the fragment shader code file\n");
    printf("\t-v <vertex-shader>\t\tRelative path to the vertex shader code file\n");
    printf("\t-m <output-mode>\t\tOutput mode for the shader rendering (window | video, default: window)\n");
    printf("\t-d <duration>\t\t\tDuration of video output in seconds\n");
}

void ParseProgramArguments(int argc, char **argv)
{
    for (int i = 0; i < argc; i++)
    {
        char *arg = argv[i];

        if (streq(arg, "--"))
        {
            break;
        }

        if (streq(arg, "-m"))
        {
            outputModeArg = argv[i + 1];
            continue;
        }

        if (streq(arg, "-f"))
        {
            fragShaderArg = argv[i + 1];
            fragShaderFlag = true;
            continue;
        }

        if (streq(arg, "-d"))
        {
            durationArg = atoi(argv[i + 1]);
            durationFlag = true;
            continue;
        }

        if (streq(arg, "-h"))
        {
            PrintProgramUsage(argv[0]);
            exit(0);
        }
    }
}

void RunWindowedRenderer(const char *windowTitle)
{
    SetTraceLogLevel(LOG_NONE);

    SetConfigFlags(FLAG_WINDOW_UNDECORATED);

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, TextFormat("%s", windowTitle));

    SetTargetFPS(fps);

    Shader shader = LoadShaderAndSetResolution(
        vertShaderFlag ? vertShaderArg : 0,
        fragShaderFlag ? fragShaderArg : 0);

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
            printf("[shaderrender] Reloading shader ...\n");

            UnloadShader(shader);

            shader = LoadShaderAndSetResolution(
                vertShaderFlag ? vertShaderArg : 0,
                fragShaderFlag ? fragShaderArg : 0);

            // Update time location (might have changed in the new shader)
            timeLoc = GetShaderLocation(shader, "u_time");

            printf("[shaderrender] Shader reloaded successfully\n");
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
    UnloadShader(shader); // Unload shader

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
    UnloadShader(shader); // Unload shader
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
