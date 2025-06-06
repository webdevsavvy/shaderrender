#version 330

// uniform that constatains the current screenspace resolution
uniform vec2  u_resolution;
uniform float u_time;

out vec4 outFragColor;

vec3 palette(float t, vec3 a, vec3 b, vec3 c, vec3 d) {
    return a + b*cos(6.28318*(c*t+d));
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution.xy) / u_resolution.y;
    vec2 uv0 = uv;
    vec3 finalColor = vec3(0.0);

    for (float i = 0.0; i < 5.0; i++) {
        uv = fract(uv * 1.5) - 0.5;

        float d = length(uv) * exp(-length(uv0));

        vec3 col = palette(
            length(uv0) + u_time * 0.33 + i * 0.33, 
            vec3(0.1451, 0.2588, 0.1216), 
            vec3(0.2902, 0.3686, 0.2745), 
            vec3(0.0431, 0.1686, 0.0196), 
            vec3(0.3373, 0.3529, 0.5686)
        );

        d = sin(d*8.0 + u_time)/8.0;
        d = abs(d);

        d = pow(0.02 / d, 2.18112);

        finalColor += col * d;
    }


    outFragColor = vec4(finalColor, 1.0);
}