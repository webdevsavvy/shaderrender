#version 330

uniform vec2 u_resolution;
uniform float u_time;

out vec4 fragColor;

mat2 Rot2D(float a) {
    float s = sin(a), c = cos(a);
    return mat2(c, -s, s, c);
}

float Star(vec2 uv, float flare) {
    float d = length(uv);
    float m = .05 / d;

    float rays = max(0.0, 1.0 - abs(uv.x * uv.y * 1000.0));
    m += rays * 1.0 * flare;
    uv *= Rot2D(3.1415 / 4);
    rays = max(0.0, 1.0 - abs(uv.x * uv.y * 1000.0));
    m += rays * 0.33 * flare;

    m *= smoothstep(1.0, 0.2, d);
    return m;
}

float Hash21(vec2 p) {
    p = fract(p*vec2(123.34, 456.21));
    p += dot(p, p+45.32);
    return fract(p.x * p.y);
}

vec3 palette(in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d) {
    return a + b * cos(6.28318 * (c * t + d));
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution.xy) / u_resolution.y;
    uv *= 5.0;

    vec3 col = vec3(0);

    vec2 gv = fract(uv) - 0.5;
    vec2 id = floor(uv);

    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            vec2 offset = vec2(x, y);
            float n = Hash21(id + offset);
            float star_size = fract(n * 4251.1);
            float star = Star(gv - offset - vec2(n, fract(n*34.)) + 0.5, sin(smoothstep(.5, .8, star_size) * 6.28 + u_time) * 0.5 + 0.5);

            col += star * star_size * palette(
                n, 
                vec3(0.5412, 0.0745, 0.0745), 
                vec3(0.2314, 0.102, 0.1412), 
                vec3(0.902, 0.0078, 0.0078), 
                vec3(0.0431, 0.0118, 0.4549)
            );
        }
    }
    
    fragColor = vec4(col, 1.0);
}