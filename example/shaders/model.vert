#version 450

#extension GL_EXT_buffer_reference : require


struct Camera {
    vec4 position;
    mat4 view;
    mat4 projection;
};

layout(set = 0, binding = 0) uniform Scene {
    Camera camera;
};


// size: 64
struct Vertex {
    vec4 position;
    vec4 normal;
    vec2 UV0;
    vec2 UV1;
    vec4 color;
};

layout(std430, buffer_reference, buffer_reference_align = 16) readonly buffer VertexBuffer
{
    Vertex vertices[];
};
layout (set = 1, binding = 0) uniform Vertices {
    VertexBuffer vertexBuffer;
};

layout(std430, buffer_reference, buffer_reference_align = 4) readonly buffer TransformBuffer
{
    mat4 transforms[];
};
layout (set = 1, binding = 1) uniform Transforms {
    TransformBuffer transformBuffer;
};


layout(push_constant) uniform Push
{
    uint transformIndex;
    uint materialIndex;
};


layout (location = 0) out vec3 out_worldPosition;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec2 out_UV0;
layout (location = 3) out vec2 out_UV1;
layout (location = 4) out vec4 out_color;


void main() {
    Vertex vertex = vertexBuffer.vertices[gl_VertexIndex];
    mat4 transform = transformBuffer.transforms[transformIndex];

    vec4 worldPosition = transform * vec4(vertex.position.xyz, 1.0);
    out_worldPosition = worldPosition.xyz / worldPosition.w;
    gl_Position = camera.projection * camera.view * vec4(out_worldPosition, 1);

    out_normal = normalize(transpose(inverse(mat3(transform))) * vertex.normal.xyz);

    out_UV0 = vertex.UV0;
    out_UV1 = vertex.UV1;

    out_color = vertex.color;
}
