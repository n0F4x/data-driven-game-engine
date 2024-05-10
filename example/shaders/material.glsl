struct TextureInfo {
    int index;
    int texCoord;
};

struct MetallicRoughness {
    vec4 baseColorFactor;
    TextureInfo baseColorTexture;
    float metallicFactor;
    float roughnessFactor;
};

struct NormalTextureInfo {
    int index;
    int texCoord;
    float scale;
};

struct OcclusionTextureInfo {
    int index;
    int texCoord;
    float strength;
};

struct Material {
    MetallicRoughness metallicRoughness;
    NormalTextureInfo normalTexture;
    OcclusionTextureInfo occlusionTexture;
    TextureInfo emissiveTexture;
    vec3 emissiveFactor;
    float alphaCutoff;
};
