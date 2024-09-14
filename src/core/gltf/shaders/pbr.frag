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


#define MIN_ROUGHNESS 0.04
#define PI 3.141592653589793
const vec3 F0 = vec3(0.04);
const vec3 LIGHT_DIR = normalize(vec3(-1, 1, -1));
const vec3 LIGHT_COLOR = vec3(1, 1, 1) * 10;

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

vec3 getNormal(Material material) {
    if (in_normal == vec3(0)) {
        return normalize(cross(dFdy(in_worldPosition), dFdx(in_worldPosition)));
    }

    if (material.normalTextureIndex == MAX_UINT_VALUE) {
        return normalize(in_normal);
    }

    vec2 normalUV = material.normalTextureTexCoord == 0 ? in_UV0 : in_UV1;

    // Perturb normal, see http://www.thetenthplanet.de/archives/1180
    // https://irrlicht.sourceforge.io/forum/viewtopic.php?t=52284
    vec3 q1 = dFdx(in_worldPosition);
    vec3 q2 = dFdy(in_worldPosition);
    vec2 st1 = dFdx(normalUV);
    vec2 st2 = dFdy(normalUV);
    mat2x3 tanSpace = mat2x3(q1, q2) * mat2(st2.y, -st1.y, -st2.x, st1.x);

    vec3 N = normalize(in_normal);
    vec3 T = normalize(normalize(tanSpace[0] - in_normal * dot(in_normal, tanSpace[0])));
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    vec4 normalSample = sampleTexture(material.normalTextureIndex, material.normalTextureTexCoord);
    vec3 tangentNormal = normalSample.xyz * 2.0 - 1.0;

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

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float microfacetDistribution(PBRInfo pbrInputs) {
    float roughnessSquared = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float NdotHSquared = pbrInputs.NdotH * pbrInputs.NdotH;
    float f = (roughnessSquared - 1.0) * NdotHSquared + 1.0;
    return roughnessSquared / (PI * f * f);
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


void main() {
    Material material = materialIndex == MAX_UINT_VALUE ? defaultMaterial : materialBuffer.materials[materialIndex];

    vec4 baseColor = getBaseColor(material);
    if (material.alphaCutoff >= 0.0) {
        if (baseColor.a < material.alphaCutoff) {
            discard;
        }
    }
    baseColor *= in_color;

    float roughness = material.pbrMetallicRoughnessRoughnessFactor;
    float metallic = material.pbrMetallicRoughnessMetallicFactor;
    if (material.pbrMetallicRoughnessMetallicRoughnessTextureIndex != MAX_UINT_VALUE) {
        vec4 metallicRoughnessSample = sampleTexture(material.pbrMetallicRoughnessMetallicRoughnessTextureIndex, material.pbrMetallicRoughnessMetallicRoughnessTextureTexCoord);
        roughness *= metallicRoughnessSample.g;
        metallic *= metallicRoughnessSample.b;
    }



    vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - F0) * (1.0 - metallic);
    float alphaRoughness = roughness * roughness;
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
    vec3 l = -normalize(LIGHT_DIR); // Vector from surface point to light
    vec3 h = normalize(l + v); // Half vector between both l and v

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
    roughness,
    metallic,
    specularEnvironmentR0,
    specularEnvironmentR90,
    alphaRoughness,
    diffuseColor,
    specularColor
    );

    // Calculate the shading terms for the microfacet specular shading model
    vec3 F = specularReflection(pbrInputs);
    float D = microfacetDistribution(pbrInputs);
    float G = geometricOcclusion(pbrInputs);

    // Calculation of analytical lighting contribution
    vec3 diffuseContrib = (1.0 - F) * diffuse(pbrInputs);
    vec3 specContrib = F * D * G / (4.0 * NdotL * NdotV);
    // Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
    vec3 color = NdotL * LIGHT_COLOR * (diffuseContrib + specContrib);

    // Apply optional PBR terms for additional (optional) shading
    if (material.occlusionTextureIndex != MAX_UINT_VALUE) {
        float occlusionStrength = material.occlusionTextureScale;
        vec4 occlusionSample = sampleTexture(material.occlusionTextureIndex, material.occlusionTextureTexCoord);
        color = mix(color, color * occlusionSample.r, occlusionStrength);
    }

    vec3 emissive = material.emissiveFactor;
    if (material.emissiveTextureIndex != MAX_UINT_VALUE) {
        vec4 emissiveSample = sampleTexture(material.emissiveTextureIndex, material.emissiveTextureTexCoord);
        emissive *= sRGBtoLinear(emissiveSample).rgb;
    };
    color += emissive;

    out_color = vec4(color, baseColor.a);
}
