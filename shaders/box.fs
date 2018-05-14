#version 330 core
in vec2 vTexture;
in vec3 vNormal;
in vec3 objPosition;

out vec4 fColor;

uniform sampler2D textureSampler;
uniform vec3 cameraPosition;
 
void main() {
    // 光照颜色
    vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    // 光源位置
    vec3 lightPosition = vec3(0.0f, 30.0f, 30.0f);

    // 环境光照强度
    float ambientStrength = 0.1f;
    // 环境光照
    vec4 ambient = ambientStrength * lightColor;

    // 光线方向
    vec3 lightDirection = normalize(lightPosition - objPosition);
    // 散射因子（光照方向与法向量夹角越小，散射强度越大）。
    float diffuseFactor = max(dot(normalize(vNormal), lightDirection), 0.0f);
    // 漫反射光照。
    vec4 diffuse = diffuseFactor * lightColor;

    // // 镜面反射强度。
    float specularStrength = 1.0f;
    // 相机方向
    vec3 cameraDirection = normalize(cameraPosition - objPosition);
    // 与光线方向相反的单位向量
    vec3 reflectDirection = reflect(-lightDirection, normalize(vNormal));
    // 镜面反射因子
    float specularFactor = pow(max(dot(cameraDirection, reflectDirection), 0.0f), 32);
    // 镜面反射光照
    vec4 specular = specularStrength * specularFactor * lightColor;

    fColor = (ambient + diffuse + specular) * texture(textureSampler, vTexture);
}