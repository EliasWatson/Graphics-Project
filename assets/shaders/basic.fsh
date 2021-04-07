#version 430

#define MAX_LIGHTS 2

#define EPSILON 0.0001
#define PI 3.1415

layout (binding=0) uniform sampler2D albedo_sampler;
layout (binding=1) uniform sampler2D roughness_metal_sampler;
layout (binding=2) uniform sampler2D normal_sampler;
layout (binding=3) uniform samplerCube irradiance_sampler;
layout (binding=4) uniform samplerCube reflection_sampler;
layout (binding=5) uniform sampler2DShadow shadow_sampler;

in vec2 texture_coord;
in vec3 frag_pos;
in vec3 frag_normal;
in mat3 frag_tbn;
in vec3 frag_light_dir[MAX_LIGHTS];
in vec4 shadow_pos;

uniform vec4 cam_pos;
uniform vec2 screen_resolution;
uniform vec2 shadow_resolution;
uniform float shadow_bias;
uniform int frame;

uniform vec4 light_pos[MAX_LIGHTS];
uniform vec4 light_ambient[MAX_LIGHTS];
uniform vec4 light_diffuse[MAX_LIGHTS];
uniform vec4 light_specular[MAX_LIGHTS];

uniform vec4 ambient_color;
uniform vec4 diffuse_color;
uniform vec4 specular_color;
uniform float shininess_factor;

uniform float albedo_sampler_contrib;
uniform float roughness_metal_sampler_contrib;
uniform float normal_sampler_contrib;

uniform float environment_intensity;

out vec4 color;

vec3 EnvBRDFApprox(vec3 f0, float NoV, float roughness) {
    vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);
    vec4 c1 = vec4(1.0, 0.0425, 1.04, -0.04);
    vec4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, pow(2.0, -9.28 * NoV)) * r.x + r.y;
    vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;
    return f0 * AB.x + AB.y;
}

float lookupShadow(float ox, float oy) {
    return textureProj(shadow_sampler, shadow_pos + vec4(
            (ox * shadow_pos.w) / shadow_resolution.x,
            (oy * shadow_pos.w) / shadow_resolution.y,
            -shadow_bias, 0.0
        ));
}

float getShadowFactor(float swidth) {
    int frame_offset = frame % 3;
    vec2 adj_pos = gl_FragCoord.xy + vec2(frame_offset & 1, (frame_offset >> 1) & 1);
    vec2 offset = mod(floor(adj_pos), 2.0) * swidth;

    float shadow_factor = 0.0;
    shadow_factor += lookupShadow(-1.5 * swidth + offset.x,  1.5 * swidth - offset.y);
    shadow_factor += lookupShadow(-1.5 * swidth + offset.x, -0.5 * swidth - offset.y);
    shadow_factor += lookupShadow( 0.5 * swidth + offset.x,  1.5 * swidth - offset.y);
    shadow_factor += lookupShadow( 0.5 * swidth + offset.x, -0.5 * swidth - offset.y);

    return shadow_factor * 0.25;
}

void main() {
    vec3 col;

    vec4 albedo_tex_color = mix(vec4(1), texture(albedo_sampler, texture_coord), albedo_sampler_contrib);
    vec3 roughness_metal_color = mix(vec3(1.0, 0.5, 0.0), texture(roughness_metal_sampler, texture_coord).rgb, roughness_metal_sampler_contrib);
    float roughness = roughness_metal_color.g;
    float metalness = roughness_metal_color.b;

    vec3 normal = texture(normal_sampler, texture_coord).xyz;
    normal = normal * 2.0 - 1.0;
    normal = normalize(frag_tbn * normal);
    normal = normalize(mix(frag_normal, normal, normal_sampler_contrib));

    vec3 V = normalize(frag_pos - cam_pos.xyz);
    vec3 R = reflect(V, normal);

    float NoV = max(0.0, dot(V, normal));
    vec3 f0 = albedo_tex_color.rgb * max(metalness, 0.04);
    vec3 diffuse = albedo_tex_color.rgb * texture(irradiance_sampler, normal).rgb * environment_intensity;
    vec3 specular = mix(vec3(1), albedo_tex_color.rgb, 1.0 - max(0.1, metalness)) * textureLod(reflection_sampler, R, roughness * 12.0).rgb * environment_intensity;
    col = mix(diffuse, specular, EnvBRDFApprox(f0, NoV, roughness));

    float not_in_shadow = getShadowFactor(2.5);
    float shadow_factor = not_in_shadow * 0.5 + 0.5;

    /*
    for(int i = 0; i < MAX_LIGHTS; ++i) {
        vec3 L = frag_light_dir[i];

        vec3 diffuse = light_diffuse[i].rgb * diffuse_color.rgb * albedo_tex_color * max(dot(normal, L), 0.0);
        vec3 specular = light_specular[i].rgb * specular_color.rgb * pow(max(dot(R, L), 0.0), shininess_factor);

        col += diffuse + specular;
    }
    */

    color = vec4(col * shadow_factor, albedo_tex_color.a);
}