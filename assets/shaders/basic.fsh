#version 430

#define MAX_LIGHTS 1

layout (binding=0) uniform sampler2D color_sampler;

in vec2 texture_coord;
in vec3 frag_pos;
in vec3 frag_normal;

uniform mat4 rot_matrix;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

uniform vec4 light_pos[MAX_LIGHTS];
uniform vec4 light_ambient[MAX_LIGHTS];
uniform vec4 light_diffuse[MAX_LIGHTS];
uniform vec4 light_specular[MAX_LIGHTS];

uniform vec4 ambient_color;
uniform vec4 diffuse_color;
uniform vec4 specular_color;
uniform float shininess_factor;

out vec4 color;

void main() {
    vec3 col;

    vec3 V = normalize(frag_pos);
    vec3 R = reflect(V, frag_normal.xyz);

    for(int i = 0; i < MAX_LIGHTS; ++i) {
        vec3 L = normalize(light_pos[i].xyz - frag_pos.xyz);

        vec3 ambient = light_ambient[i].rgb * ambient_color.rgb;
        vec3 diffuse = light_diffuse[i].rgb * diffuse_color.rgb * max(dot(frag_normal, L), 0.0);
        vec3 specular = light_specular[i].rgb * specular_color.rgb * pow(max(dot(R, L), 0.0), shininess_factor);

        col += ambient + diffuse + specular;
    }

    color = vec4(col, 1);
}