#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

uniform mat4 rot_matrix;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform float tf;

out vec2 texture_coord;
out vec3 frag_pos;
out vec3 frag_normal;

void main() {
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);

    texture_coord = uv;
    frag_pos = (mv_matrix * vec4(position, 1)).xyz;
    frag_normal = (rot_matrix * vec4(normal, 1)).xyz;
}