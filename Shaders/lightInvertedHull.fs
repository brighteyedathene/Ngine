#version 450

out vec4 FragColor;


uniform vec3 lineColour;

in vec3 Normal;
in vec3 FragPos;


void main()
{
    FragColor = vec4(lineColour, 1);
    

    
}