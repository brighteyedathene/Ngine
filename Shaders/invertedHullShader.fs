#version 450

out vec4 FragColor;


struct Light {
    vec3 position;
    vec3 colour;
};
uniform Light light; 

uniform vec3 viewPos;

uniform float edgeThreshold;

in vec3 Normal;
in vec3 FragPos;


void main()
{


    // light direction and normal
    vec3 norm = normalize(Normal);
    vec3 lightDir = light.position - FragPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    // lambert
    float lambertian = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);

    vec3 result;


    // edge detector
    float edgy = dot(norm, viewDir);
    if (edgy < edgeThreshold)
    {
    	result = vec3(0.0);
    }
    else
    {
    	result = vec3(1.0);
    }
    //FragColor = vec4(result, 1.0);

    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}