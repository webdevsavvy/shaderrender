#ifndef FFMPEG_H_
#define FFMPEG_H_

#include <stddef.h>

typedef struct FFMPEG
{
    int pid;
    int pipe;
} FFMPEG;

/**
 * @brief Initializes the pipe with ffmpeg and returns the pipe handle.
 * 
 * @param width 
 * @param height 
 * @param fps 
 * @return int - The pipe handle
 */
FFMPEG *InitFFMPEG(size_t width, size_t height, size_t fps);

/**
 * @brief Sends a frame through the pipe to ffmpeg.
 * 
 * @param ffmpeg 
 * @param data 
 * @param width 
 * @param height 
 */
void SendFrameFFMPEG(FFMPEG *ffmpeg, void *data, size_t width, size_t height);

/**
 * @brief Frees the memory used by ffmepg and waits till the pipe is closed.
 * 
 * @param ffmpeg 
 */
void UnloadFFMPEG(FFMPEG *ffmpeg);

#endif // FFMPEG_H_