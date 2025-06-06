#version 330

#define PI 3.14159265359
#define TAU 6.28318530718

uniform vec2 u_resolution;
uniform float u_time;

out vec4 fragColor;

float EggSDF(in vec2 uv, in float ra, in float rb);

float EggZero(vec2 p);

float EggOne(vec2 p);

float EggTwo(vec2 p);

float EggThree(vec2 p);

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution.xy) / u_resolution.y;
    
    vec3 color = vec3(0);
    
    float e0 = EggZero(uv);
    float e1 = EggOne(uv);
    float e2 = EggTwo(uv);
    float e3 = EggThree(uv);

    color.r += min(e0, e1);

    color.b = e1;

    color.rg += e2 + e3;

    fragColor = vec4(color, 1.0);
}

float EggSDF(in vec2 p, in float ra, in float rb) {
    const float k = sqrt(3.0);
    p.x = abs(p.x);
    float r = ra - rb;
    return ((p.y < 0.0) ? length(vec2(p.x, p.y)) - r : (k * (p.x + r) < p.y) ? length(vec2(p.x, p.y - k * r)) : length(vec2(p.x + r, p.y)) - 2.0 * r) - rb;
}

float EggZero(vec2 p) {
    // Animate the ra and rb parameters
    float animatedRa = 0.03 + 0.01 * sin(u_time);
    float animatedRb = -0.6 + 0.15 * cos(u_time * TAU * 1.3);

    float d = EggSDF(p, animatedRa, animatedRb);

    d = smoothstep(0.1, 0.1, d);

    d = 0.1 / abs(d);

    return d;
}

float EggOne(vec2 p) {
    // Animate the ra and rb parameters
    float animatedRa = 0.03 + 0.01 * sin(u_time);
    float animatedRb = -0.6 + 0.15 * cos(u_time * TAU * 1.3);

    float d = EggSDF(p, animatedRa, animatedRb);

    d = smoothstep(0.05, 0.1, d);

    d = 0.01 / abs(d);

    return d;
}

float EggTwo(vec2 p) {
    // Animate the ra and rb parameters
    float animatedRa = 0.03 + 0.01 * sin(u_time);
    float animatedRb = -0.6 + 0.15 * cos(u_time * TAU * 1.3);

    float d = EggSDF(p, animatedRa, animatedRb);

    d = smoothstep(0.1, 0.3, d);

    d = 0.01 / abs(d);

    return d;
}

float EggThree(vec2 p) {
    // Animate the ra and rb parameters
    float animatedRa = 0.03 + 0.01 * sin(u_time);
    float animatedRb = -0.6 + 0.15 * cos(u_time * TAU * 1.3);

    float d = EggSDF(p, animatedRa, animatedRb);

    d = smoothstep(0.1, 0.3, d);

    d = 0.01 / abs(d);

    return d;
}
