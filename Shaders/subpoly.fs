#version 450

out vec4 FragColor;


uniform vec3 objectColor;
uniform vec3 viewPos;

uniform float edgeThreshold;

in vec3 Normal;
in vec3 FragPos;

in float viewDotNormal;
in vec3 vColour;

in float lambert;

void main()
{
    // light direction and normal
    //vec3 lightDir = light.position - FragPos;
    //float distance = length(lightDir);
    //lightDir = normalize(lightDir);

    // diffuse
    //float lambertian = max(dot(Normal, lightDir), 0.0);



    //if (abs(viewDotNormal) < (1 - lambertian) * edgeThreshold){
    if (viewDotNormal <= min((1 - lambert) * edgeThreshold, edgeThreshold)){
    //if (viewDotNormal <=  edgeThreshold){
    	//FragColor = vec4(1, 0, 0, 1);
    	//FragColor = vec4(vec3(lambert), 1.0);
    	FragColor = vec4(0, 0, 0, 1);
    }
    else{
    	FragColor = vec4(1);
    	//FragColor = vec4(vec3(0.5 + lambert/2), 1);
    }

    //FragColor = vec4(vec3(0.5 + viewDotNormal/2), 1);


    //FragColor = vec4(vColour.r, 0.5, 0.5, 1.0);
 
}