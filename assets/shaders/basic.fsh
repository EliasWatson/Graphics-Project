#version 430

#define MAX_LIGHTS 2

#define EPSILON 0.0001
#define PI 3.1415

layout (binding=0) uniform sampler2D albedo_sampler;
layout (binding=1) uniform sampler2D roughness_sampler;
layout (binding=2) uniform sampler2D normal_sampler;
layout (binding=3) uniform sampler2D irradiance_sampler;
layout (binding=4) uniform sampler2D reflection_sampler;

in vec2 texture_coord;
in vec3 frag_pos;
in vec3 frag_normal;
in mat3 frag_tbn;
in vec3 frag_light_dir[MAX_LIGHTS];

uniform vec4 light_pos[MAX_LIGHTS];
uniform vec4 light_ambient[MAX_LIGHTS];
uniform vec4 light_diffuse[MAX_LIGHTS];
uniform vec4 light_specular[MAX_LIGHTS];

uniform vec4 ambient_color;
uniform vec4 diffuse_color;
uniform vec4 specular_color;
uniform float shininess_factor;

uniform float albedo_sampler_contrib;
uniform float roughness_sampler_contrib;
uniform float normal_sampler_contrib;

uniform float environment_intensity;

out vec4 color;

vec2 vec3_to_spherical(vec3 dir) {
    dir = dir.xzy * vec3(1, 1, -1);
    if(abs(dir.x) < EPSILON && abs(dir.y) < EPSILON) return vec2(0.0);

    float theta = atan(dir.y / dir.x);
    float phi = atan(length(dir.xy) / dir.z);

    // TODO: Get rid of branches
    if(dir.x < 0.0 && dir.y >= 0.0 && abs(theta) < EPSILON) theta = PI;
    else if(dir.x < 0.0 && dir.y < 0.0 && sign(theta) > 0.0) theta -= PI;
    else if(dir.x < 0.0 && dir.y > 0.0 && sign(theta) < 0.0) theta += PI;

    return vec2(theta, phi);
}

float schlick(float f0, float f90, float NoV) {
    return f0 + ((f90 - f0) * pow(1.0 - NoV, 5.0));
}

float fresnel(float NoV) {
    return schlick(0.0, 1.0, NoV);
}

void main() {
    vec3 col;

    vec3 albedo_tex_color = mix(vec3(1), texture(albedo_sampler, texture_coord).rgb, albedo_sampler_contrib);
    float roughness_tex_factor = mix(0.5, 1.0 - texture(roughness_sampler, texture_coord).x, roughness_sampler_contrib);

    vec3 normal = texture(normal_sampler, texture_coord).xyz;
    normal = normal * 2.0 - 1.0;
    normal = normalize(frag_tbn * normal);
    normal = normalize(mix(frag_normal, normal, normal_sampler_contrib));

    vec3 V = normalize(frag_pos);
    vec3 R = reflect(V, normal);

    /*
    for(int i = 0; i < MAX_LIGHTS; ++i) {
        vec3 L = frag_light_dir[i];

        vec3 ambient = light_ambient[i].rgb * ambient_color.rgb * albedo_tex_color;
        vec3 diffuse = light_diffuse[i].rgb * diffuse_color.rgb * albedo_tex_color * max(dot(normal, L), 0.0);
        vec3 specular = light_specular[i].rgb * specular_color.rgb * pow(max(dot(R, L), 0.0), shininess_factor);

        col += ambient + diffuse + specular;
    }
    */

    float NoV = max(0.0, dot(V, normal));
    vec3 f0 = max(albedo_tex_color * 0.0, 0.04);
    vec3 diffuse = albedo_tex_color * texture(irradiance_sampler, vec3_to_spherical(normal) / (PI * 2.0)).rgb * environment_intensity;
    vec3 specular = textureLod(reflection_sampler, vec3_to_spherical(R) / (PI * 2.0), roughness_tex_factor * 12.0).rgb * environment_intensity;

    color = vec4(mix(diffuse, specular, f0 + fresnel(NoV)), 1.0);
}