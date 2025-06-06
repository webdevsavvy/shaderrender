#version 330

// uniform that constatains the current screenspace resolution
uniform vec2  u_resolution;
uniform float u_time;

out vec4 fragColor;

vec3 palette(in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d) {
    return a + b*cos(6.28318*(c*t+d));
}

float sdEquilateralTriangle(in vec2 p, in float r )
{
    const float k = sqrt(3.0);
    p.x = abs(p.x) - r;
    p.y = p.y + r/k;
    if( p.x+k*p.y>0.0 ) p = vec2(p.x-k*p.y,-k*p.x-p.y)/2.0;
    p.x -= clamp( p.x, -2.0*r, 0.0 );
    return -length(p)*sign(p.y);
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution.xy) / u_resolution.y;
    vec2 uv0 = uv;
    vec3 finalColor = vec3(0.0);

    for(float i = 0.0; i < 41.0; i++) {
        float d = sdEquilateralTriangle(uv, 4);

        vec3 col = palette(
            length(uv0) + u_time * 0.4, 
            vec3(0.7059, 0.8549, 0.6745), 
            vec3(0.7882, 0.5804, 0.6824), 
            vec3(0.651, 0.6588, 0.7216), 
            vec3(0.7725, 0.1804, 0.0)
        );

        d = sin(d * 8.0 - u_time) / 8.0;
        d = abs(d) ;

        d = pow(0.01 / d, 1.2);

        col *= d;

        finalColor += col * d; 
    }

	fragColor = vec4(finalColor,1.0);
}