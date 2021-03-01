#version 430

#define MAX_LIGHTS 2

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;
uniform mat4 norm_matrix;
uniform float tf;

uniform vec4 light_pos[MAX_LIGHTS];

out vec2 texture_coord;
out vec3 frag_pos;
out vec3 frag_normal;
out mat3 frag_tbn;
out vec3 frag_light_dir[MAX_LIGHTS];

void main() {
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);

    texture_coord = uv;
    frag_pos = (mv_matrix * vec4(position, 1)).xyz;
    frag_normal = (norm_matrix * vec4(normal, 1)).xyz;

    vec3 bitangent = cross(normal, tangent);
    frag_tbn = mat3(
        normalize((mv_matrix * vec4(tangent,   0.0)).xyz),
        normalize((mv_matrix * vec4(bitangent, 0.0)).xyz),
        normalize((mv_matrix * vec4(normal,    0.0)).xyz)
    );

    for(int i = 0; i < MAX_LIGHTS; ++i) {
        frag_light_dir[i] = normalize(light_pos[i].xyz - frag_pos);
    }
}