#version 450

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal; // this should be normalized
//in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 viewPos;

uniform vec3 refractiveIndexRGB;
uniform float refractiveIndex;
uniform float bias;
uniform float scale;
uniform float power;

void main()
{
	float ratio = 1/refractiveIndex;

	vec3 I = normalize(FragPos - viewPos);
	
	vec3 refl = reflect(I, Normal);
	vec3 reflectSample = texture(skybox, refl).rgb;
	
	// Simple refraction
	//vec3 refr = refract(I, Normal, ratio);
	//vec3 refractSample = texture(skybox, refr).rgb;

	// Refraction with chromatic dispersion
	vec3 refrR = refract(I, Normal, refractiveIndexRGB.r);
	vec3 refrG = refract(I, Normal, refractiveIndexRGB.g);
	vec3 refrB = refract(I, Normal, refractiveIndexRGB.b);

	vec3 refractSample;
	refractSample.r = texture(skybox, refrR).r;
	refractSample.g = texture(skybox, refrG).g;
	refractSample.b = texture(skybox, refrB).b;

	// Reflection Coefficient
	float reflectionCoefficient = max(0, min(1, bias + scale * pow(1 + dot(I, Normal), power) ));


	vec3 result = reflectionCoefficient * reflectSample + ( 1 - reflectionCoefficient) * refractSample;

	FragColor = vec4(result, 1.0);

}