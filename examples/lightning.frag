#version 330

uniform vec2 u_resolution;
uniform float u_time;

out vec4 fragColor;

vec3 effect_color = vec3(0.3333, 0.0471, 0.5686);
int octave_count = 10;
float amp_start = 0.5;
float amp_coeff = 0.5;
float freq_coeff = 2.0;
float speed = 0.5;

float hash21(vec2 uv) {
    return fract(cos(mod(dot(uv, vec2(12.9898, 8.1414)), 3.14)) * 43758.5453);
}

vec2 hash22(vec2 uv) {
    uv = vec2(dot(uv, vec2(127.1, 311.7)), dot(uv, vec2(269.5, 183.3)));
    return 2.0 * fract(sin(uv) * 43758.5453123) - 1;
}

float noise(vec2 uv) {
    vec2 iuv = floor(uv);
    vec2 fuv = fract(uv);
    vec2 blur = smoothstep(0.0, 1.0, fuv);
    return mix(
        mix(
            dot(hash22(iuv + vec2(0.0, 0.0)), fuv - vec2(0.0, 0.0)),
            dot(hash22(iuv + vec2(1.0, 0.0)), fuv - vec2(1.0, 0.0)), 
            blur.x
        ),
        mix(
            dot(hash22(iuv + vec2(0.0, 1.0)), fuv - vec2(0.0, 1.0)), 
            dot(hash22(iuv + vec2(1.0, 1.0)), fuv - vec2(1.0, 1.0)),
            blur.x
        ),
        blur.y
    ) + 0.5;
}

float fbm(vec2 uv, int octaves) {
    float value = 0.0;
    float amplitude = amp_start;
    for(int i = 0; i < octaves; i++) {
        value += amplitude * noise(uv);
        uv *= freq_coeff;
        amplitude *= amp_coeff;
    }
    return value;
}


void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution.xy) / u_resolution.y;

    uv += 2.0 * fbm(uv + u_time * speed, octave_count) - 1;

    float d = abs(uv.x);
   
    vec3 col = effect_color * mix(0.0, 0.9 * abs(sin(u_time * speed * 6.28)), hash21(vec2(u_time))) / d;
    
    fragColor = vec4(col, 1.0);
}