#version 430

layout (location = 0) in vec3 position;

out vec3 view_pos;

uniform mat4 proj_matrix;
uniform mat4 v_matrix;

void main() {
    mat4 v_mat_mod = v_matrix;
    v_mat_mod[3] = vec4(0, 0, 0, 1);

    gl_Position = proj_matrix * v_mat_mod * vec4(position, 1.0);
    view_pos = position;
}