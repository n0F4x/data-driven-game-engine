#define MAX_UINT_VALUE = 0xffffffff;

struct TextureInfo {
    uint index;
    uint texCoord;
};

struct PbrMetallicRoughness {
    vec4 baseColorFactor;
    TextureInfo baseColorTexture;
    float metallicFactor;
    float roughnessFactor;
    TextureInfo metallicRoughnessTexture;
};

struct NormalTextureInfo {
    uint index;
    uint texCoord;
    float scale;
};

struct OcclusionTextureInfo {
    uint index;
    uint texCoord;
    float strength;
};

struct Material {
    PbrMetallicRoughness pbrMetallicRoughness;
    NormalTextureInfo normalTexture;
    OcclusionTextureInfo occlusionTexture;
    TextureInfo emissiveTexture;
    vec3 emissiveFactor;
    float alphaCutoff;
};
