#include "Material.h"



Material::Material()
{
	ambient = glm::vec3(0.0f);
	diffuse = glm::vec3(0.4f);
	specular = glm::vec3(0.5f);
	shininess = 12.0f;
}


Material::~Material()
{
}
