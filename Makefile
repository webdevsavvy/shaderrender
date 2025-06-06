CC=gcc
CFLAGS=-g -Wall
RAYLIB_FLAGS=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: shaderrender

shaderrender: shaderrender.c ffmpeg_linux.c
	$(CC) $(CFLAGS) -o $@ $^ $(RAYLIB_FLAGS)

clean:
	rm screnderer