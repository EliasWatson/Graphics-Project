#version 430

layout (binding=0) uniform sampler2D color_sampler;

in vec2 texture_coord;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform float tf;

out vec4 color;

void main() {
    color = texture(color_sampler, texture_coord);
}