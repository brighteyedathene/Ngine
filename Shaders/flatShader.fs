#version 450

out vec4 FragColor;
 
struct TextureMaterial {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct Material {
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
 
uniform vec3 viewPos;
uniform int numShades;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

vec3 FlatShading()
{
    // light direction and normal
    vec3 norm = normalize(Normal);
    vec3 lightDir = light.position - FragPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Intensity
    float amb = 0.2;
    float diff = max(dot(norm, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float intensity = amb + (diff + spec);
    intensity = max(0.2, ceil(intensity * numShades)/numShades);

    vec3 result = intensity * material.diffuse * light.colour;

    return result;
}

void main()
{
	vec3 result = FlatShading();
    result = light.colour;

    FragColor = vec4(result, 1.0);
}