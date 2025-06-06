# Shaderrender

Shaderrender is a tool for rendering vertex and fragment shaders into a window or to a video.

### Usage

To render to a window use:

```shell
./shaderrender -m window -f examples/circles.frag
```

To render to a video use:

```shell
./shaderrender -m video -f examples/circles.frag -d 20
```

Result:

![output from video render](./output.gif)