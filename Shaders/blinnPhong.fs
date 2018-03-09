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

in vec3 Normal;
in vec3 FragPos;

in vec3 vColour;

void main()
{
    // ambient
    vec3 ambient = material.ambient;

    // light direction and normal
    vec3 norm = normalize(Normal);
    vec3 lightDir = light.position - FragPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    // diffuse
    float lambertian = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lambertian * material.diffuse * light.colour / distance*distance;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfDir, norm), 0.0), material.shininess);
    vec3 specular = spec * material.specular * light.colour / distance;

    vec3 result = ambient + diffuse + specular;

    //FragColor = vec4(result, 1);
    FragColor = vec4(1);
    //FragColor = vec4(vColour.r, 0.5, 0.5, 1.0);
 
}