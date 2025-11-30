#version 450


layout (binding = 0) uniform Camera {
    vec4 position;
    mat4 view;
    mat4 projection;
} camera;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_uv;

layout (location = 0) out vec2 out_UV;


void main() {
    gl_Position = camera.projection * camera.view * vec4(in_position, 1);

    out_UV = in_uv;
}
