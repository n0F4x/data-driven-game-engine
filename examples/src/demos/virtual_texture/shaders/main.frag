#version 450

#extension GL_ARB_sparse_texture2: enable
#extension GL_ARB_sparse_texture_clamp: enable


layout (binding = 1) uniform sampler2D texSampler;


layout (location = 0) in vec2 in_UV;

layout (location = 0) out vec4 out_color;


void main() {
    vec4 color = vec4(0.f);

    float lod = textureQueryLod(texSampler, in_UV).x;
    int residencyCode = sparseTextureLodARB(texSampler, in_UV, lod, color);

    while (!sparseTexelsResidentARB(residencyCode))
    {
        lod++;
        residencyCode = sparseTextureClampARB(texSampler, in_UV, lod, color);
    }

    out_color = color;
}
