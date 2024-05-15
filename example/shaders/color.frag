#version 450

#extension GL_EXT_buffer_reference: require
#extension GL_EXT_nonuniform_qualifier: require

#include "material.glsl"
#include "utility.glsl"

layout (location = 0) in vec3 in_worldPosition;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_UV0;
layout (location = 3) in vec2 in_UV1;
layout (location = 4) in vec4 in_color;


struct Camera {
    mat4 view;
    mat4 projection;
};

layout (set = 0, binding = 0) uniform Scene {
    Camera camera;
};

layout (set = 1, binding = 2) uniform sampler defaultSampler;

struct Texture {
    uint imageIndex;
    uint samplerIndex;
};
layout (std430, buffer_reference, buffer_reference_align = 8) readonly buffer TextureBuffer
{
    Texture textures[];
};
layout (set = 1, binding = 3) uniform Textures {
    TextureBuffer textureBuffer_;
};

layout (set = 1, binding = 4) uniform DefaultMaterial {
    Material defaultMaterial;
};

layout (std430, buffer_reference, buffer_reference_align = 64) readonly buffer MaterialBuffer
{
    Material materials[];
};
layout (set = 1, binding = 5) uniform Materials {
    MaterialBuffer materialBuffer;
};

layout (set = 2, binding = 0) uniform texture2D images[];
layout (set = 3, binding = 0) uniform sampler samplers[];


layout (push_constant) uniform Push
{
    uint transformIndex;
    uint materialIndex;
};


layout (location = 0) out vec4 out_color;


#define MIN_ROUGHNESS 0.04


vec4 sample_texture(Texture texture_, vec2 UV) {
    if (texture_.samplerIndex == MAX_UINT_VALUE) {
        return texture(sampler2D(images[texture_.imageIndex], defaultSampler), UV);
    }
    return texture(sampler2D(images[texture_.imageIndex], samplers[texture_.samplerIndex]), UV);
}


// Based on: https://github.com/SaschaWillems/Vulkan-glTF-PBR
void main() {
    Material material = materialIndex == MAX_UINT_VALUE ? defaultMaterial : materialBuffer.materials[materialIndex];

    vec4 baseColor;
    if (material.pbrMetallicRoughnessBaseColorTextureIndex != MAX_UINT_VALUE) {
        Texture colorTexture = textureBuffer_.textures[material.pbrMetallicRoughnessBaseColorTextureIndex];
        vec2 colorUV = material.pbrMetallicRoughnessBaseColorTextureTexCoord == 0 ? in_UV0 : in_UV1;
        vec4 colorSample = sample_texture(colorTexture, colorUV);
        baseColor = SRGBtoLINEAR(colorSample) * material.pbrMetallicRoughnessBaseColorFactor;
    } else {
        baseColor = material.pbrMetallicRoughnessBaseColorFactor;
    }

    if (material.alphaCutoff >= 0.0) {
        if (baseColor.a < material.alphaCutoff) {
            discard;
        }
    }

    baseColor *= in_color;

    out_color = baseColor;
}
