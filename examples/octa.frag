#version 330

uniform vec2 u_resolution;
uniform float u_time;

out vec4 fragColor;

float sdSphere(in vec3 p, float r) {
    return length(p) - r;
}

float sdBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sdOctahedron(vec3 p, float s) {
    p = abs(p);
    return (p.x + p.y + p.z - s) * 0.57735027;
}

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
    p.z += u_time * .4;

    p.xy  = (fract(p.xy) - .5);
    p.z = mod(p.z, .25) - .125;

    float oct = sdOctahedron(p, .15);

    return oct;
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution.xy) / u_resolution.y;

    // Initialization
    vec3 ro = vec3(0, 0, -3);
    vec3 rd = normalize(vec3(uv, 1));
    vec3 col = vec3(0);

    float t = 0.0; // total distance travelled

    // Raymarching
    int i;
    for (i = 0; i < 40; i++) {
        vec3 p = ro + rd * t;

        p.xy *= rot2D(t * .2);
        p.y += sin(t) * .33;

        float d = map(p);

        t += d;

        if (d < .001)
            break;
        if (t > 100.)
            break;
    }

    col = palette(
        t * .04 + float(i) * .005, 
        vec3(0.5, 0.5, 0.5),
        vec3(0.5, 0.5, 0.5),
        vec3(1.0, 1.0, 1.0),
        vec3(0.263, 0.416, 0.557)
    );

    fragColor = vec4(col, 1);
}