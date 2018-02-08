#version 450

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal; // this should be normalized
in vec2 TexCoords;

in mat3 TBN;

uniform samplerCube skybox;
uniform vec3 viewPos;

uniform sampler2D texturemap;
uniform sampler2D normalmap;

uniform vec3 refractiveIndexRGB;
uniform float refractiveIndex;
uniform float bias;
uniform float scale;
uniform float power;

vec2 GetParallaxOffset(vec2 texCoords, vec3 viewDir);

float HEIGHT_SCALE = 3.0;


void main()
{
	vec3 I = normalize(FragPos - viewPos);
	
	//vec2 parallaxTexCoords = GetParallaxOffset(TexCoords, I);
	vec2 parallaxTexCoords = TexCoords;
	
	// calculate normal from normal map and TBN
	vec3 normalSample = texture(normalmap, parallaxTexCoords).rgb;
	normalSample = normalize(normalSample * 2.0 - 1.0);
	normalSample = normalize(TBN * normalSample);
	
	// Texture sample
	vec3 textureSample = texture(texturemap, parallaxTexCoords).rbg;

	// Reflection sample
	vec3 refl = reflect(I, normalSample);
	vec3 reflectSample = texture(skybox, refl).rgb;

	// Reflection Coefficient
	float reflectionCoefficient = max(0, min(1, bias + scale * pow(1 + dot(I, normalSample), power) ));


	vec3 result = reflectionCoefficient * reflectSample + ( 1 - reflectionCoefficient) * textureSample;
	//vec3 result = textureSample;

	FragColor = vec4(result, 1.0);

}

vec2 GetParallaxOffset(vec2 texCoords, vec3 viewDir)
{
	float height = texture(normalmap, texCoords).r;
	vec2 p = viewDir.xy / viewDir.z * (height * HEIGHT_SCALE);
	return texCoords - p;
}