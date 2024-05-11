#version 450

#extension GL_EXT_buffer_reference : require

#include "material.glsl"

layout (location = 0) in vec4 in_color;


struct Camera {
    mat4 view;
    mat4 projection;
};

layout (set = 0, binding = 0) uniform Scene {
    Camera camera;
};

layout(set = 1, binding = 2) uniform texture2D images[];
layout(set = 1, binding = 3) uniform sampler samplers[];

layout(std430, buffer_reference, buffer_reference_align = 16) readonly buffer MaterialBuffer
{
    Material materials[];
};
layout(set = 1, binding = 4) uniform Materials {
    MaterialBuffer materialBuffer;
};


layout(location = 0) out vec4 out_color;

void main() {
    out_color = in_color;
}
