#version 430

uniform float time;

out vec3 vert_color;

const vec4 verts[] = vec4[](
    vec4( 0.0,  0.5, 0.0, 1.0),
    vec4(-0.5, -0.5, 0.0, 1.0),
    vec4( 0.5, -0.5, 0.0, 1.0)
);

const vec3 cols[] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    gl_Position = verts[gl_VertexID];
    gl_Position.x += sin(time) * 0.5;
    gl_Position.y += sin(time * 0.726) * 0.5;

    vert_color = cols[gl_VertexID];
}