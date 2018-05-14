#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture;
layout (location = 2) in vec3 normal;

out vec2 vTexture;
out vec3 vNormal;
out vec3 objPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vTexture = texture;
    // 用正规矩阵修复不等比缩放破坏法线导致的光照扭曲。
    vNormal = mat3(transpose(inverse(model))) * normal;
    // 对象的世界坐标。
    objPosition = vec3(model * vec4(position, 1.0f));
}