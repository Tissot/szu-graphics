#version 330 core
in vec3 vTexture;
out vec4 fColor;

uniform samplerCube skyBox;

void main () {
    fColor = texture(skyBox, vTexture);
}