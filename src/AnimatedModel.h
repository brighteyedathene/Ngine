#pragma once

#include <map>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AssortedUtils.h"

using namespace std;

class AnimatedModel
{
public:
	AnimatedModel();
	~AnimatedModel();

	bool LoadMesh(const string& filename);
	bool InitFromScene(const aiScene* pScene, const string& filename);
	void Render();

	unsigned int numBones() const
	{
		return m_NumBones;
	}

	void BoneTransform(float timeInSeconds, vector<glm::mat4>& transforms);

	void Clear();

private:

#define NUM_BONES_PER_VEREX 4

	struct BoneInfo
	{
		Matrix4f BoneOffset;
		Matrix4f FinalTransformation;

		BoneInfo()
		{
			BoneOffset.SetZero();
			FinalTransformation.SetZero();
		}
	};

	struct VertexBoneData
	{
		uint IDs[NUM_BONES_PER_VEREX];
		float Weights[NUM_BONES_PER_VEREX];

		VertexBoneData()
		{
			Reset();
		};

		void Reset()
		{
			ZERO_MEM(IDs);
			ZERO_MEM(Weights);
		}

		void AddBoneData(uint BoneID, float Weight);
	};


#define INVALID_MATERIAL 0xFFFFFFFF

	enum VB_TYPES
	{
		INDEX_BUFFER,
		POS_VB,
		NORMAL_VB,
		TEXCOORD_VB,
		BONE_VB,
		NUM_VBs
	};

	GLuint m_VAO;
	GLuint m_Buffers[NUM_VBs];

	struct MeshEntry
	{
		MeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
	};

	vector<MeshEntry> m_Entries;
	vector<Texture*> m_Textures;

	unsigned int m_NumBones;
	glm::mat4 glm_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));

	aiMatrix4x4 m_GlobalInverseTransform;
	aiMatrix4x4 myAssMatrix;
	glm::mat4 matfromass;

	const aiScene* m_pScene;
	Assimp::Importer m_Importer;
};
