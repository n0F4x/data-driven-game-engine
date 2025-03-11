#define MAX_UINT_VALUE 0xffffffff

// size: 128
struct Material {
    // size: 16
    vec4 pbrMetallicRoughnessBaseColorFactor;

    // size: 4
    uint pbrMetallicRoughnessBaseColorTextureIndex;
    // size: 4
    uint pbrMetallicRoughnessBaseColorTextureTexCoord;
    // size: 4
    float pbrMetallicRoughnessMetallicFactor;
    // size: 4
    float pbrMetallicRoughnessRoughnessFactor;

    // size: 4
    uint pbrMetallicRoughnessMetallicRoughnessTextureIndex;
    // size: 4
    uint pbrMetallicRoughnessMetallicRoughnessTextureTexCoord;
    // size: 4
    uint normalTextureIndex;
    // size: 4
    uint normalTextureTexCoord;

    // size: 4
    float normalTextureScale;
    // size: 4
    uint occlusionTextureIndex;
    // size: 4
    uint occlusionTextureTexCoord;
    // size: 4
    float occlusionTextureScale;

    // size: 4
    uint emissiveTextureIndex;
    // size: 4
    uint emissiveTextureTexCoord;
    // size: 8
    vec2 _padding0;

    // size: 12
    vec3 emissiveFactor;
    // size: 4
    float alphaCutoff;

    // size: 16
    vec4 _padding1;

    // size: 16
    vec4 _padding2;
};
