#version 430

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec4 color;

void main() {
    color = vec4(1.0, 0.0, 0.0, 1.0);
}