#version 330

uniform vec2 u_resolution;
uniform float u_time;

out vec4 fragColor;

float smin(float a, float b, float k) {
    float h = max(k - abs(a - b), 0.0) / k;
    return min(a, b) - h * h * h * k * (1.0 / 6.0);
}

mat2 rot2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

vec3 palette(in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d) {
    return a + b * cos(6.28318 * (c * t + d));
}

float map(vec3 p) {
    return length(p) - 1;
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution.xy) / u_resolution.y;

    // Initialization
    vec3 ro = vec3(0, 0, -3);
    vec3 rd = normalize(vec3(uv, 1));
    vec3 col = vec3(0);

    float t = 0.0; // total distance travelled

    // Raymarching
    for (int i = 0; i < 80; i++) {
        vec3 p = ro + rd * t;

        float d = map(p);

        t += d;

        if (d < .001)
            break;
        if (t > 100.)
            break;
    }

    col = vec3(t * .2);
    fragColor = vec4(col, 1);
}