#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ffmpeg.h"

#define READ_END 0
#define WRITE_END 1

FFMPEG *InitFFMPEG(size_t width, size_t height, size_t fps) {
    int pipefd[2];

    if (pipe(pipefd) < 0)
    {
        fprintf(stderr, "ERROR: could not create a pipe: %s\n", strerror(errno));
        exit(1);
    }

    pid_t child = fork();
    if (child < 0)
    {
        fprintf(stderr, "ERROR: could not fork a child: %s\n", strerror(errno));
        exit(1);
    }

    if (child == 0)
    {
        if (dup2(pipefd[READ_END], STDIN_FILENO) < 0)
        {
            fprintf(stderr, "ERROR: could not reopen read end of pipe as stdin: %s\n", strerror(errno));
            exit(1);
        }
        close(pipefd[WRITE_END]);

        char resolution[64];
        snprintf(resolution, sizeof(resolution), "%zux%zu", width, height);
        char framerate[64];
        snprintf(framerate, sizeof(framerate), "%zu", fps);

        int ret = execlp("ffmpeg",
        "ffmpeg",
        "-loglevel", "info",
        "-y",

        "-f", "rawvideo",
        "-pix_fmt", "rgba",
        "-s", resolution,
        "-r", framerate,
        "-i", "-",

        "-c:v", "libx264",
        "-preset", "veryslow",
        "-crf", "0",
        "-qp", "0",
        "-pix_fmt", "yuv444p",
        "output.mp4",

        NULL);
        if (ret < 0)
        {
            fprintf(stderr, "ERROR: could not run ffmpeg as a child process: %s\n", strerror(errno));
            exit(1);
        }
        assert(0 && "unreachable");
    }

    close(pipefd[READ_END]);

    FFMPEG *ffmpeg = malloc(sizeof(FFMPEG));
    assert(ffmpeg != NULL && "Buy MORE RAM lol!!");
    ffmpeg->pid = child;
    ffmpeg->pipe = pipefd[WRITE_END];
    return ffmpeg;
}

void UnloadFFMPEG(FFMPEG *ffmpeg) {
    close(ffmpeg->pipe);
    waitpid(ffmpeg->pid, NULL, 0);
    free(ffmpeg);
}

void SendFrameFFMPEG(FFMPEG *ffmpeg, void *data, size_t width, size_t height)
{
    write(ffmpeg->pipe, data, sizeof(uint32_t) * width * height);
}