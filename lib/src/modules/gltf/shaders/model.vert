#version 450

#extension GL_EXT_buffer_reference: require


struct Camera {
    vec4 position;
    mat4 view;
    mat4 projection;
};

layout (set = 0, binding = 0) uniform Scene {
    Camera camera;
};


const uint vertexSize = 14;
const uint positionOffset = 0;
const uint normalOffset = 3;
const uint uv0Offset = 6;
const uint uv1Offset = 8;
const uint colorOffset = 10;
layout (std430, buffer_reference, buffer_reference_align = 4) readonly buffer VertexBuffer {
    float vertices[];
};
layout (set = 1, binding = 0) uniform Vertices {
    VertexBuffer vertexBuffer;
};

layout (std430, buffer_reference, buffer_reference_align = 4) readonly buffer TransformBuffer {
    mat4 transforms[];
};
layout (set = 1, binding = 1) uniform Transforms {
    TransformBuffer transformBuffer;
};


layout (push_constant) uniform Push {
    uint transformIndex;
    uint materialIndex;
};


layout (location = 0) out vec3 out_worldPosition;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec2 out_UV0;
layout (location = 3) out vec2 out_UV1;
layout (location = 4) out vec4 out_color;


void main() {
    vec3 position = vec3(
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + positionOffset],
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + positionOffset + 1],
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + positionOffset + 2]
    );
    vec3 normal = vec3(
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + normalOffset],
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + normalOffset + 1],
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + normalOffset + 2]
    );
    vec2 uv0 = vec2(
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + uv0Offset],
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + uv0Offset + 1]
    );
    vec2 uv1 = vec2(
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + uv1Offset],
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + uv1Offset + 1]
    );
    vec4 color = vec4(
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + colorOffset],
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + colorOffset + 1],
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + colorOffset + 2],
    vertexBuffer.vertices[vertexSize * gl_VertexIndex + colorOffset + 3]
    );

    mat4 transform = transformBuffer.transforms[transformIndex];


    vec4 worldPosition = transform * vec4(position, 1.0);
    out_worldPosition = worldPosition.xyz / worldPosition.w;
    gl_Position = camera.projection * camera.view * vec4(out_worldPosition, 1);
    gl_PointSize = 1.0f;

    out_normal = normalize(transpose(inverse(mat3(transform))) * normal);

    out_UV0 = uv0;
    out_UV1 = uv1;

    out_color = color;
}
