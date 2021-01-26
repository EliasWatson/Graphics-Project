#version 430

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

out vec3 vert_color;

void main() {
    gl_Position = verts[gl_VertexID];
    vert_color = cols[gl_VertexID];
}