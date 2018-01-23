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
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
 
uniform vec3 objectColor;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

int NUM_SHADES = 2;

vec3 FlatShading()
{

    // light direction and normal
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Intensity
    float amb = 0;
    float diff = max(dot(norm, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float intensity = amb + diff + spec;
    intensity = max(0.2, ceil(intensity * NUM_SHADES)/NUM_SHADES);

    vec3 result = intensity * material.diffuse;

    return result;

}

void main()
{
	vec3 result = FlatShading();

    FragColor = vec4(result, 1.0);
}