// Based on: https://github.com/SaschaWillems/Vulkan-glTF-PBR
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


// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
struct PBRInfo
{
    float NdotL;                  // cos angle between normal and light direction
    float NdotV;                  // cos angle between normal and view direction
    float NdotH;                  // cos angle between normal and half vector
    float LdotH;                  // cos angle between light direction and half vector
    float VdotH;                  // cos angle between view direction and half vector
    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
    float metalness;              // metallic value at the surface
    vec3 reflectance0;            // full reflectance color (normal incidence angle)
    vec3 reflectance90;           // reflectance color at grazing angle
    float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3 diffuseColor;            // color contribution from diffuse lighting
    vec3 specularColor;           // color contribution from specular lighting
};

#define MIN_ROUGHNESS 0.04
#define PI 3.141592653589793
const vec3 F0 = vec3(0.04);
const vec3 LIGHT_DIR = normalize(vec3(1, -1, 1));
const vec3 LIGHT_COLOR = vec3(1, 1, 1) * 5;


vec4 sample_texture(Texture texture_, vec2 UV) {
    if (texture_.samplerIndex == MAX_UINT_VALUE) {
        return texture(sampler2D(images[texture_.imageIndex], defaultSampler), UV);
    }
    return texture(sampler2D(images[texture_.imageIndex], samplers[texture_.samplerIndex]), UV);
}


// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
vec3 getNormal(Material material) {
    if (material.normalTextureIndex == MAX_UINT_VALUE) {
        return normalize(in_normal);
    }

    // Perturb normal, see http://www.thetenthplanet.de/archives/1180
    Texture normalTexture = textureBuffer_.textures[material.normalTextureIndex];
    vec2 normalUV = material.normalTextureTexCoord == 0 ? in_UV0 : in_UV1;
    vec4 normalSample = sample_texture(normalTexture, normalUV);
    vec3 tangentNormal = normalSample.xyz * 2.0 - 1.0;

    vec3 q1 = dFdx(in_worldPosition);
    vec3 q2 = dFdy(in_worldPosition);
    vec2 st1 = dFdx(normalUV);
    vec2 st2 = dFdy(normalUV);

    vec3 N = normalize(in_normal);
    vec3 T = normalize(q1 * st2.t - q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
vec3 diffuse(PBRInfo pbrInputs) {
    return pbrInputs.diffuseColor / PI;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
vec3 specularReflection(PBRInfo pbrInputs) {
    return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
float geometricOcclusion(PBRInfo pbrInputs) {
    float NdotL = pbrInputs.NdotL;
    float NdotV = pbrInputs.NdotV;
    float r = pbrInputs.alphaRoughness;

    float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
    float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
    return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float microfacetDistribution(PBRInfo pbrInputs) {
    float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
    return roughnessSq / (PI * f * f);
}


void main() {
    Material material = materialIndex == MAX_UINT_VALUE ? defaultMaterial : materialBuffer.materials[materialIndex];

    // The albedo may be defined from a base texture or a flat color
    vec4 baseColor;
    if (material.pbrMetallicRoughnessBaseColorTextureIndex != MAX_UINT_VALUE) {
        Texture colorTexture = textureBuffer_.textures[material.pbrMetallicRoughnessBaseColorTextureIndex];
        vec2 colorUV = material.pbrMetallicRoughnessBaseColorTextureTexCoord == 0 ? in_UV0 : in_UV1;
        vec4 colorSample = sample_texture(colorTexture, colorUV);
        baseColor = SRGBtoLINEAR(colorSample) * material.pbrMetallicRoughnessBaseColorFactor;
    } else {
        baseColor = material.pbrMetallicRoughnessBaseColorFactor;
    }
    baseColor *= in_color;

    if (material.alphaCutoff >= 0.0) {
        if (baseColor.a < material.alphaCutoff) {
            discard;
        }
    }

    // Metallic and Roughness material properties are packed together
    // In glTF, these factors can be specified by fixed scalar values
    // or from a metallic-roughness map
    float perceptualRoughness = material.pbrMetallicRoughnessRoughnessFactor;
    float metallic = material.pbrMetallicRoughnessMetallicFactor;
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


    vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - F0) * (1.0 - metallic);
    float alphaRoughness = perceptualRoughness * perceptualRoughness;
    vec3 specularColor = mix(F0, baseColor.rgb, metallic);

    // Compute reflectance.
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    vec3 specularEnvironmentR0 = specularColor.rgb;
    vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

    vec3 n = getNormal(material);
    n.y *= -1.0f;
    vec3 v = normalize(camera.position.xyz - in_worldPosition); // Vector from surface point to camera
    vec3 l = normalize(LIGHT_DIR); // Vector from surface point to light
    vec3 h = normalize(l + v); // Half vector between both l and v
    vec3 reflection = normalize(reflect(-v, n));

    float NdotL = clamp(dot(n, l), 0.001, 1.0);
    float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
    float NdotH = clamp(dot(n, h), 0.0, 1.0);
    float LdotH = clamp(dot(l, h), 0.0, 1.0);
    float VdotH = clamp(dot(v, h), 0.0, 1.0);

    PBRInfo pbrInputs = PBRInfo(
        NdotL,
        NdotV,
        NdotH,
        LdotH,
        VdotH,
        perceptualRoughness,
        metallic,
        specularEnvironmentR0,
        specularEnvironmentR90,
        alphaRoughness,
        diffuseColor,
        specularColor
    );

    // Calculate the shading terms for the microfacet specular shading model
    vec3 F = specularReflection(pbrInputs);
    float G = geometricOcclusion(pbrInputs);
    float D = microfacetDistribution(pbrInputs);

    // Calculation of analytical lighting contribution
    vec3 diffuseContrib = (1.0 - F) * diffuse(pbrInputs);
    vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);
    // Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
    vec3 color = NdotL * LIGHT_COLOR * (diffuseContrib + specContrib);

    // Apply optional PBR terms for additional (optional) shading
    if (material.occlusionTextureIndex != MAX_UINT_VALUE) {
        float occlusionStrength = material.occlusionTextureScale;
        Texture occlusionTexture = textureBuffer_.textures[material.occlusionTextureIndex];
        vec2 occlusionUV = material.occlusionTextureTexCoord == 0 ? in_UV0 : in_UV1;
        vec4 occlusionSample = sample_texture(occlusionTexture, occlusionUV);
        color = mix(color, color * occlusionSample.r, occlusionStrength);
    }

    vec3 emissive = material.emissiveFactor;
    if (material.emissiveTextureIndex != MAX_UINT_VALUE) {
        Texture emissiveTexture = textureBuffer_.textures[material.emissiveTextureIndex];
        vec2 emissiveUV = material.emissiveTextureTexCoord == 0 ? in_UV0 : in_UV1;
        vec4 emissiveSample = sample_texture(emissiveTexture, emissiveUV);
        emissive *= SRGBtoLINEAR(emissiveSample).rgb;
    };
    color += emissive;

    out_color = vec4(color, baseColor.a);
}