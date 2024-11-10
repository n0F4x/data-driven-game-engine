#version 450

#extension GL_ARB_sparse_texture2: enable


layout (binding = 1) uniform sampler2D texSampler;


layout (location = 0) in vec2 in_UV;

layout (location = 0) out vec4 out_color;


void main() {
    vec4 color = vec4(0.0);

    int lod = 0;
    int residencyCode = sparseTextureLodARB(texSampler, in_UV, lod, color);

    while (!sparseTexelsResidentARB(residencyCode))
    {
        lod++;
        residencyCode = sparseTextureLodARB(texSampler, in_UV, lod, color);
    }

    //    out_color = vec4(1.f - float(lod) / 5.f);
    out_color = color;
}
