#version 450

#extension GL_ARB_sparse_texture2: enable
#extension GL_EXT_buffer_reference: enable


layout (binding = 1) uniform sampler2D texSampler;

layout (binding = 2) uniform ImageInfo {
    uvec2 baseExtent;
    uvec2 granularity;
    uint blockCount;
    uint mipTailFirstLod;
} imageInfo;

layout (std430, buffer_reference, buffer_reference_align = 4) buffer BlockRequestBuffer {
    uint blockRequests[];
};
layout (binding = 3) uniform BlockRequest {
    BlockRequestBuffer blockRequestBuffer;
};


layout (location = 0) in vec2 in_UV;

layout (location = 0) out vec4 out_color;


uvec2 aligned_division(uvec2 divident, uvec2 divisor) {
    return uvec2(
    divident.x / divisor.x + (((divident.x % divisor.x) != 0u) ? 1u : 0u),
    divident.y / divisor.y + (((divident.y % divisor.y) != 0u) ? 1u : 0u)
    );
}

uint mip_offset_index(uvec2 bind_counts, vec2 mipOffset) {
    return
    (uint(floor(mipOffset.y)) / imageInfo.granularity.y) * bind_counts.x
    + uint(floor(mipOffset.x)) / imageInfo.granularity.x
    ;
}


void main() {
    vec4 color;

    float lod = textureQueryLod(texSampler, in_UV).x;
    int residencyCode = sparseTextureLodARB(texSampler, in_UV, lod, color);

    if (!sparseTexelsResidentARB(residencyCode))
    {
        residencyCode = sparseTextureLodARB(texSampler, in_UV, floor(lod) + 1, color);
        if (!sparseTexelsResidentARB(residencyCode)) {
            residencyCode = sparseTextureLodARB(texSampler, in_UV, ceil(lod) - 1, color);
            if (!sparseTexelsResidentARB(residencyCode)) {
                residencyCode = sparseTextureLodARB(texSampler, in_UV, imageInfo.mipTailFirstLod, color);
            }
        }
    }

    uint flooredLod = uint(floor(lod));
    uint ceiledLod = uint(ceil(lod));

    uvec2 mipExtent = imageInfo.baseExtent;
    uint blockIndex = 0;
    uvec2 bind_counts;
    for (uint i = 0; i < flooredLod; i++) {
        bind_counts = aligned_division(mipExtent, imageInfo.granularity);
        blockIndex += bind_counts.x * bind_counts.y;
        mipExtent.x = max(mipExtent.x / 2u, 1u);
        mipExtent.y = max(mipExtent.y / 2u, 1u);
    }
    bind_counts = aligned_division(mipExtent, imageInfo.granularity);
    uint flooredBlockIndex = blockIndex
    + mip_offset_index(bind_counts, in_UV * mipExtent)
    ;
    if (flooredBlockIndex < imageInfo.blockCount) {
        atomicOr(blockRequestBuffer.blockRequests[flooredBlockIndex], 1);
    }

    if (ceiledLod != flooredLod) {
        blockIndex += bind_counts.x * bind_counts.y;
        mipExtent.x = max(mipExtent.x / 2, 1);
        mipExtent.y = max(mipExtent.y / 2, 1);
        bind_counts = aligned_division(mipExtent, imageInfo.granularity);

        uint ceiledBlockIndex = blockIndex + mip_offset_index(bind_counts, in_UV * mipExtent);
        atomicOr(blockRequestBuffer.blockRequests[ceiledBlockIndex], 1);
    }

    out_color = color;
}
