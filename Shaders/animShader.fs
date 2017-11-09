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

smooth in float DEBUG_ZERO_WEIGHT;

void main()
{
    // ambient
    vec3 ambient = material.ambient * light.ambient;

    // light direction and normal
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * light.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * light.specular;

    vec3 result = ambient + diffuse + specular;

    //if (DEBUG_ZERO_WEIGHT > 0.2)
    FragColor = vec4(result, 1.0);
    //else
    //FragColor = vec4(1.0, 0.8, 0.1, 1.0);
}