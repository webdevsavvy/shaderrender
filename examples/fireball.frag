#version 330

#define PI 3.14159265359
#define TAU 6.28318530718

uniform vec2 u_resolution;
uniform float u_time;

out vec4 fragColor;

float CircleSDF(vec2 p, float r);

float PerlinNoise(vec2 uv);

vec2 Hash22(vec2 uv);

float FBM(vec2 uv, int octaves, float amp_start, float amp_coef, float freq_coef);

float SmoothMin(float a, float b, float k);

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution.xy) / u_resolution.y;

    float d = CircleSDF(uv, 0);

    float fbm = FBM(uv - vec2(0, u_time), 8, 0.5, 0.5, 2.0);

    d = smoothstep(0.0, sin(0.3 * TAU * u_time) * 0.5 + 0.8, d);

    d = 0.1 / d;

    float o = SmoothMin(d, fbm, 0.05);

    vec3 color = vec3(o, 0.0, 0.0);

    fragColor = vec4(color, 1.0);
}

float CircleSDF(vec2 p, float r) {
    return length(p) - r;
}

float SmoothMin(float a, float b, float k) {
    float h = max(k - abs(a - b), 0.0) / k;
    return min(a, b) - h * h * h * k * (1.0 / 6.0);
}

float PerlinNoise(vec2 uv) {
    vec2 iuv = floor(uv);
    vec2 fuv = fract(uv);
    vec2 blur = smoothstep(0.0, 1.0, fuv);
    return mix(
        mix(
            dot(Hash22(iuv + vec2(0.0)), fuv - vec2(0.0)),
            dot(Hash22(iuv + vec2(1.0, 0.0)), fuv - vec2(1.0, 0.0)), 
            blur.x
        ),
        mix(
            dot(Hash22(iuv + vec2(0.0, 1.0)), fuv - vec2(0.0, 1.0)), 
            dot(Hash22(iuv + vec2(1.0, 1.0)), fuv - vec2(1.0, 1.0)),
            blur.x
        ),
        blur.y
    ) + 0.5;
}

vec2 Hash22(vec2 uv) {
    uv = vec2(dot(uv, vec2(127.1, 311.7)), dot(uv, vec2(269.5, 183.3)));
    return 2.0 * fract(sin(uv) * 43758.5453123) - 1;
}

float FBM(vec2 uv, int octaves, float amp_start, float amp_coeff, float freq_coeff) {
    float value = 0.0;
    float amplitude = amp_start;
    for (int i = 0; i < octaves; i++) {
        value += amplitude * PerlinNoise(uv);
        uv *= freq_coeff;
        amplitude *= amp_coeff;
    }
    return value;
}
