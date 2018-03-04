#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <assimp/Importer.hpp>

#include "Texture.h"
#include "Shader.h"


using namespace std;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	glm::vec3 Colour;
};

class Mesh
{
public:
	// Mesh data
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	~Mesh();
	void Draw(Shader shader);

private:
	// Render data
	unsigned int VBO, EBO;

	void SetupMesh();

};

