#version 430

layout (binding=4) uniform samplerCube reflection_sampler;

in vec3 view_pos;

uniform float sky_brightness;

out vec4 color;

void main() {
    vec3 view_dir = normalize(view_pos);
    color = texture(reflection_sampler, view_dir) * sky_brightness;
    color.w = 1.0;
}