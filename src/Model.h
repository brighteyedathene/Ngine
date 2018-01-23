#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "IDrawable.h"

class Model : public IDrawable
{
public:
	Model(const char* path);
	~Model();
	void Draw(Shader* pShader);
private:
	// model data
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;

	void LoadModel(string path);
	void ProcessNode(aiNode * node, const aiScene *scene);
	Mesh ProcessMesh(aiMesh * mesh, const aiScene *scene);
	vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
	unsigned int LoadTextureFromFile(const char *path, const string &directory, bool gamma = false);
};

