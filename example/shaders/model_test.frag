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

    float perceptualRoughness;
    float metallic;
    vec3 diffuseColor;
    vec4 baseColor;

    vec3 f0 = vec3(0.04);


    if (material.alphaCutoff >= 0.0) {
        if (material.pbrMetallicRoughnessBaseColorTextureIndex != MAX_UINT_VALUE) {
            Texture colorTexture = textureBuffer_.textures[material.pbrMetallicRoughnessBaseColorTextureIndex];
            vec2 colorUV = material.pbrMetallicRoughnessBaseColorTextureTexCoord == 0 ? in_UV0 : in_UV1;
            vec4 colorSample = sample_texture(colorTexture, colorUV);
            baseColor = SRGBtoLINEAR(colorSample) * material.pbrMetallicRoughnessBaseColorFactor;
        } else {
            baseColor = material.pbrMetallicRoughnessBaseColorFactor;
        }
        if (baseColor.a < material.alphaCutoff) {
            discard;
        }
    }

    // Metallic and Roughness material properties are packed together
    // In glTF, these factors can be specified by fixed scalar values
    // or from a metallic-roughness map
    perceptualRoughness = material.pbrMetallicRoughnessRoughnessFactor;
    metallic = material.pbrMetallicRoughnessMetallicFactor;
    if (material.pbrMetallicRoughnessMetallicRoughnessTextureIndex != MAX_UINT_VALUE) {
        // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
        // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
        Texture metallicRoughnessTexture = textureBuffer_.textures[material.pbrMetallicRoughnessMetallicRoughnessTextureIndex];
        vec2 metallicRoughnessUV = material.pbrMetallicRoughnessMetallicRoughnessTextureTexCoord == 0 ? in_UV0 : in_UV1;
        vec4 metallicRoughnessSample = sample_texture(metallicRoughnessTexture, metallicRoughnessUV);
        perceptualRoughness = metallicRoughnessSample.g * perceptualRoughness;
        metallic = metallicRoughnessSample.b * metallic;
    } else {
        perceptualRoughness = clamp(perceptualRoughness, MIN_ROUGHNESS, 1.0);
        metallic = clamp(metallic, 0.0, 1.0);
    }
    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].

    // The albedo may be defined from a base texture or a flat color
    if (material.pbrMetallicRoughnessBaseColorTextureIndex != MAX_UINT_VALUE) {
        Texture colorTexture = textureBuffer_.textures[material.pbrMetallicRoughnessBaseColorTextureIndex];
        vec2 colorUV = material.pbrMetallicRoughnessBaseColorTextureTexCoord == 0 ? in_UV0 : in_UV1;
        vec4 colorSample = sample_texture(colorTexture, colorUV);
        baseColor = SRGBtoLINEAR(colorSample) * material.pbrMetallicRoughnessBaseColorFactor;
    } else {
        baseColor = material.pbrMetallicRoughnessBaseColorFactor;
    }

    baseColor *= in_color;

    out_color = baseColor;
}
