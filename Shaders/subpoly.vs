#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec3 aColour;


struct Light {
    vec3 position;
    vec3 colour;
};
uniform Light light;  

uniform mat4 projectionview;
uniform mat4 model;

uniform vec3 viewPos;

out vec3 Normal;
out vec3 FragPos;

out float viewDotNormal;
out float viewDistance;
out vec3 vColour;

out float lambert;

void main()
{

    FragPos = vec3(model * vec4(aPos, 1.0f));
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    vColour = aColour;

    vec3 lightDir = light.position - FragPos;
    lambert = max(dot(Normal, lightDir), 0.0);



    vec3 viewDirection = FragPos - viewPos;
    viewDotNormal = dot(-viewDirection, Normal);

    viewDistance = length(viewDirection);
    viewDotNormal /= viewDistance;

    gl_Position = projectionview * vec4(FragPos, 1.0);
} 