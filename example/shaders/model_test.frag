#version 450

#extension GL_EXT_buffer_reference : require

#include "material.glsl"

layout (location = 0) in vec3 in_worldPosition;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_UV0;
layout (location = 3) in vec2 in_UV1;
layout (location = 4) in vec4 in_color;


struct Camera {
    mat4 view;
    mat4 projection;
};

layout(set = 0, binding = 0) uniform Scene {
    Camera camera;
};

layout(set = 1, binding = 2) uniform sampler defaultSampler;

struct Texture{
    uint samplerIndex;
    uint imageIndex;
};
layout(std430, buffer_reference, buffer_reference_align = 1) readonly buffer TextureBuffer
{
    Texture textures[];
};
layout(set = 1, binding = 3) uniform Textures {
    TextureBuffer textureBuffer_;
};

layout(std430, buffer_reference, buffer_reference_align = 16) readonly buffer MaterialBuffer
{
    Material materials[];
};
layout(set = 1, binding = 4) uniform Materials {
    MaterialBuffer materialBuffer;
};

layout(set = 2, binding = 0) uniform texture2D images[];
layout(set = 3, binding = 0) uniform sampler samplers[];


layout(location = 0) out vec4 out_color;


void main() {
    if (in_color != vec4(0, 0, 0, 0)) {
        out_color = in_color;
    } else {
        //        out_color = vec4((in_normal + vec3(in_UV0, 0)) / 2.0, 1);
        out_color = vec4(in_normal, 1);
    }
}
