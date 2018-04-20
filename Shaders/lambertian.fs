#version 450

out vec4 FragColor; 

struct Light {
    vec3 position;
};

uniform Light light;  
 
uniform vec3 objectColor;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;

in vec3 vColour;

void main()
{
    // light direction and normal
    vec3 norm = normalize(Normal);
    vec3 lightDir = light.position - FragPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float lambertian = max(dot(norm, lightDir), 0.0);

    FragColor = vec4(vec3(lambertian), 1.0);
}