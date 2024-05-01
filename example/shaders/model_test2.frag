#version 450

layout (location = 0) in vec4 in_color;


struct Camera {
    mat4 view;
    mat4 projection;
};

layout (set = 0, binding = 0) uniform Scene {
    Camera camera;
};


layout (location = 0) out vec4 out_color;

void main() {
    out_color = in_color;
}
