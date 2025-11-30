#version 450

#extension GL_ARB_sparse_texture2: enable


layout (binding = 1) uniform sampler2D texSampler;


layout (location = 0) in vec2 in_UV;

layout (location = 0) out vec4 out_color;


void main() {
    vec4 color = vec4(0.f);

    float lod = textureQueryLod(texSampler, in_UV).x;
    int residencyCode = sparseTextureLodARB(texSampler, in_UV, lod, color);

    float minLod = floor(lod);
    while (!sparseTexelsResidentARB(residencyCode))
    {
        minLod++;
        residencyCode = sparseTextureLodARB(texSampler, in_UV, minLod, color);
    }

    out_color = color;
}
