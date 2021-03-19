#version 430

#define EPSILON 0.0001
#define PI 3.1415

layout (binding=4) uniform sampler2D reflection_sampler;

in vec3 view_pos;

uniform float sky_brightness;

out vec4 color;

vec2 vec3_to_spherical(vec3 dir) {
    dir = dir.xzy * vec3(1, 1, -1);
    if(abs(dir.x) < EPSILON && abs(dir.y) < EPSILON) return vec2(0.0);

    float theta = atan(dir.y / dir.x);
    float phi = atan(length(dir.xy) / dir.z);

    // TODO: Get rid of branches
    if(dir.x < 0.0 && dir.y >= 0.0 && abs(theta) < EPSILON) theta = PI;
    else if(dir.x < 0.0 && dir.y < 0.0 && sign(theta) > 0.0) theta -= PI;
    else if(dir.x < 0.0 && dir.y > 0.0 && sign(theta) < 0.0) theta += PI;

    return vec2(theta, phi);
}

void main() {
    vec3 view_dir = normalize(view_pos);
    color = texture(reflection_sampler, vec3_to_spherical(view_dir) / (PI * 2.0)) * sky_brightness;
    color.w = 1.0;
}