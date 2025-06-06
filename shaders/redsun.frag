#version 330

uniform vec2 u_resolution;
uniform float u_time;

out vec4 fragColor;

float CircleSDF(vec2 p, float r) {
    return length(p) - r;
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution.xy) / u_resolution.y;

    float d = CircleSDF(uv, 0);

    d = smoothstep(0.1, 0.2, d);

    d = 0.1 / abs(sin(10 * d + u_time + uv.x * uv.y));

    vec3 color = vec3(d / 1.35, 2 * d / 5, 2 * d);

    fragColor = vec4(color, 1.0);
}