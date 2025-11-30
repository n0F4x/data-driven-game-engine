#version 450


layout (binding = 1) uniform sampler2D texSampler;


layout (location = 0) in vec2 in_UV;

layout (location = 0) out vec4 out_color;


void main() {
    vec4 color = vec4(0.f);

    out_color = texture(texSampler, in_UV);
}
