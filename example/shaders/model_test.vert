#version 450

#extension GL_EXT_buffer_reference : require


struct Camera {
    mat4 view;
    mat4 projection;
};

layout(set = 0, binding = 0) uniform Scene {
    Camera camera;
};


struct Vertex {
    vec4 position;
    vec4 normal;
    vec4 tangent;
    vec2 uv_0;
    vec2 uv_1;
    vec4 color;
};

layout(std430, buffer_reference, buffer_reference_align = 16) readonly buffer VertexBuffer
{
    Vertex vertices[];
};

layout(std430, buffer_reference, buffer_reference_align = 4) readonly buffer TransformBuffer
{
    mat4 transforms[];
};

layout (set = 1, binding = 0) uniform Model {
    VertexBuffer vertexBuffer;
    TransformBuffer transformBuffer;
};


layout(push_constant) uniform Push
{
    uint transform_index;
};


layout(location = 0) out vec4 out_color;


void main() {
    Vertex vertex = vertexBuffer.vertices[gl_VertexIndex];
    mat4 transform = transformBuffer.transforms[transform_index];

    vec4 world_position = transform * vec4(vertex.position.xyz, 1.0);
    gl_Position = camera.projection * camera.view * world_position;

    if (vertex.color != vec4(0, 0, 0, 0)) {
        out_color = vertex.color;
    } else {
        out_color = vec4((vertex.normal.xyz + vec3(vertex.uv_0, 0)) / 2, 1);
    }
}
