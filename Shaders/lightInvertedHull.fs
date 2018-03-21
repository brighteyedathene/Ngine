#version 450

out vec4 FragColor;

uniform float edgeThreshold;

in vec3 Normal;
in vec3 FragPos;

in float lambert;

void main()
{


    //FragColor = vec4(vec3(lambert), 1.0);
    FragColor = vec4(vec3(0.0), 1.0);
}