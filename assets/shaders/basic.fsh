#version 430

layout (binding=0) uniform sampler2D color_sampler;

in vec2 texture_coord;
in vec3 frag_normal;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform float tf;

out vec4 color;

void main() {
    float diffuse = dot(frag_normal, vec3(0, 1, 0)) * 0.5 + 0.5;

    color = texture(color_sampler, texture_coord);
    color.rgb *= diffuse;
}