#version 430

layout (location = 0) in vec3 position;

uniform mat4 v_matrix;
uniform mat4 proj_matrix;
uniform float tf;

out vec3 vertex_color;

mat4 buildRotateX(float rad);
mat4 buildRotateY(float rad);
mat4 buildRotateZ(float rad);
mat4 buildTranslate(vec3 pos);

void main() {
    float i = gl_InstanceID + tf;
    vec3 offset = vec3(
        sin(203.0 * i/8000.0),
        sin(301.0 * i/4001.0),
        sin(400.0 * i/6004.0)
    ) * 400.0;

    mat4 rotX = buildRotateX(1.75 * i);
    mat4 rotY = buildRotateY(1.75 * i);
    mat4 rotZ = buildRotateZ(1.75 * i);
    mat4 translate = buildTranslate(offset);

    mat4 m_matrix = translate * rotX * rotY * rotZ;
    mat4 mv_matrix = v_matrix * m_matrix;

    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
    vertex_color = position.xyz * 0.5 + 0.5;
}

mat4 buildRotateX(float rad) {
    return transpose(mat4(
        vec4(1,        0,         0, 0),
        vec4(0, cos(rad), -sin(rad), 0),
        vec4(0, sin(rad),  cos(rad), 0),
        vec4(0,        0,         0, 1)
    ));
}

mat4 buildRotateY(float rad) {
    return transpose(mat4(
        vec4( cos(rad), 0, sin(rad), 0),
        vec4(        0, 1,        0, 0),
        vec4(-sin(rad), 0, cos(rad), 0),
        vec4(        0, 0,        0, 1)
    ));
}

mat4 buildRotateZ(float rad) {
    return transpose(mat4(
        vec4(cos(rad), -sin(rad), 0, 0),
        vec4(sin(rad),  cos(rad), 0, 0),
        vec4(       0,         0, 1, 0),
        vec4(       0,         0, 0, 1)
    ));
}

mat4 buildTranslate(vec3 pos) {
    return transpose(mat4(
        vec4(1, 0, 0, -pos.x),
        vec4(0, 1, 0, -pos.y),
        vec4(0, 0, 1, -pos.z),
        vec4(0, 0, 0,      1)
    ));
}
