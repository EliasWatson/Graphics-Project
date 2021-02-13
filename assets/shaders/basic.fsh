#version 430

layout (binding=0) uniform sampler2D samp;

in vec2 texture_coord;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec4 color;

void main() {
    color = texture(samp, texture_coord);
}