#version 450

out vec4 FragColor;
 
struct Material {
    sampler2D diffuseTexture;
    sampler2D specular;
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

vec3 NormalShading()
{
    // ambient
    vec3 ambient = vec3(texture(material.diffuseTexture, TexCoords)) * light.ambient;

    // light direction and normal
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(texture(material.diffuseTexture, TexCoords)) * light.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * vec3(texture(material.specular, TexCoords)) * light.specular;

    vec3 result = ambient + diffuse + specular;

    return result;

}


vec3 FlatShading()
{

    // light direction and normal
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Intensity
    float amb = 0.2;
    float diff = max(dot(norm, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float intensity = amb + diff + spec;
    intensity = ceil(intensity * NUM_SHADES)/NUM_SHADES;

    vec3 result = intensity * vec3(texture(material.diffuseTexture, TexCoords));

    return result;

}

void main()
{
	vec3 result = FlatShading();

    FragColor = vec4(result, 1.0);
}