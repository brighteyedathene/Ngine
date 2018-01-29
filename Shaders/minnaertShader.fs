#version 450

out vec4 FragColor;
 
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 colour;
};

uniform Material material;
uniform Light light;  
 
uniform vec3 objectColor;
uniform vec3 viewPos;

uniform float darkenCoefficient;

in vec3 Normal;
in vec3 FragPos;


void main()
{
    // ambient
    vec3 ambient = material.ambient;

    // light direction and normal
    vec3 norm = normalize(Normal);
    vec3 lightDir = light.position - FragPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    vec3 viewDir = normalize(viewPos - FragPos);

    // minnaert
    float nDotL = dot(norm, lightDir);
    float nDotE = dot(norm, viewDir);

    float intensity = max(clamp(nDotL, 0.0, 1.0) * pow(nDotL * nDotE, darkenCoefficient) / distance, 0);

    vec3 result = ambient + intensity * material.diffuse * light.colour;

    FragColor = vec4(result, 1.0);
}
