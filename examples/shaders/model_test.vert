#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec3 in_normal;

layout(push_constant) uniform Camera {
    mat4 projection;
    mat4 view;
} camera;

layout (set = 0, binding = 0) uniform NodeUBO {
    mat4 matrix;
} node;

layout(location = 0) out vec3 out_color;

void main() {
    vec4 world_position = node.matrix * vec4(in_position, 1.0);
    gl_Position = camera.projection * camera.view * world_position;

    out_color = in_normal;
}
