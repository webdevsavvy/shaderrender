#version 330

uniform vec2 u_resolution;
uniform float u_time;

out vec4 fragColor;

// Generates a starfield effect
vec3 starfield(vec2 uv, float time) {

    vec3 col = vec3(0);

    float s = length(uv);
    float m = .02 / s;

    return m * col;
}

float sdPentagon(in vec2 p, in float r) {
    const vec3 k = vec3(0.809016994, 0.587785252, 0.726542528);
    p.x = abs(p.x);
    p -= 2.0 * min(dot(vec2(-k.x, k.y), p), 0.0) * vec2(-k.x, k.y);
    p -= 2.0 * min(dot(vec2(k.x, k.y), p), 0.0) * vec2(k.x, k.y);
    p -= vec2(clamp(p.x, -r * k.z, r * k.z), r);
    return length(p) * sign(p.y);
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution.xy) / u_resolution.y;

    float p = sdPentagon(vec2(uv.x, -uv.y), .25);
    float pentagonMask = step(p, 0.0);
    vec3 pentagonColor = vec3(1.0) * pentagonMask;

    vec3 s = starfield(uv, u_time);

    vec3 color = mix(s, pentagonColor, pentagonMask);

    fragColor = vec4(color, 1.0);
}