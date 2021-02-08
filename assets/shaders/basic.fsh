#version 430

in vec3 vertex_color;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec4 color;

void main() {
    color = vec4(vertex_color, 1.0);
}