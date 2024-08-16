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
    vec4 position;
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
layout (std430, buffer_reference, buffer_reference_align = 8) readonly buffer TextureBuffer {
    Texture textures[];
};
layout (set = 1, binding = 3) uniform Textures {
    TextureBuffer textureBuffer_;
};

layout (set = 1, binding = 4) uniform DefaultMaterial {
    Material defaultMaterial;
};

layout (std430, buffer_reference, buffer_reference_align = 128) readonly buffer MaterialBuffer {
    Material materials[];
};
layout (set = 1, binding = 5) uniform Materials {
    MaterialBuffer materialBuffer;
};

layout (set = 2, binding = 0) uniform texture2D images[];
layout (set = 3, binding = 0) uniform sampler samplers[];


layout (push_constant) uniform Push {
    uint transformIndex;
    uint materialIndex;
};


layout (location = 0) out vec4 out_color;


vec4 sampleTexture(uint textureIndex, uint UVIndex) {
    Texture texture_ = textureBuffer_.textures[textureIndex];
    vec2 UV = UVIndex == 0 ? in_UV0 : in_UV1;

    return texture_.samplerIndex == MAX_UINT_VALUE ?
    texture(sampler2D(images[texture_.imageIndex], defaultSampler), UV) :
    texture(sampler2D(images[texture_.imageIndex], samplers[texture_.samplerIndex]), UV);
}

vec4 getBaseColor(Material material) {
    vec4 baseColor = material.pbrMetallicRoughnessBaseColorFactor;
    if (material.pbrMetallicRoughnessBaseColorTextureIndex != MAX_UINT_VALUE) {
        vec4 colorSample = sampleTexture(material.pbrMetallicRoughnessBaseColorTextureIndex, material.pbrMetallicRoughnessBaseColorTextureTexCoord);
        baseColor *= sRGBtoLinear(colorSample);
    }

    return baseColor;
}

void main() {
    Material material = materialIndex == MAX_UINT_VALUE ? defaultMaterial : materialBuffer.materials[materialIndex];

    vec4 baseColor = getBaseColor(material);
    if (material.alphaCutoff >= 0.0) {
        if (baseColor.a < material.alphaCutoff) {
            discard;
        }
    }
    baseColor *= in_color;

    out_color = baseColor;
}
