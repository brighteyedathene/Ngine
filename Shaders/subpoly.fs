#version 450

out vec4 FragColor;


struct Light {
    vec3 position;
    vec3 colour;
};
uniform Light light;  

uniform float edgeThreshold;

uniform vec3 lineColour;

in vec3 Normal;
in vec3 FragPos;

in float viewDotNormal;

in float lambert;

void main()
{
    // Find lambertian value at this fragment
    vec3 lightDir = light.position - FragPos;
    lightDir = normalize(lightDir);
    float lambertian = max(dot(Normal, lightDir), 0.0);

	if (viewDotNormal <= min((1 - lambertian) * edgeThreshold, edgeThreshold)){
    	FragColor = vec4(lineColour, 1);
    }
    else{
    	FragColor = vec4(1);
    }

 
}