#pragma once

#include <map>
#include <vector>
#include <string>
#include <cassert>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "SATexture.h"


// some handy shit from ogldev's thing
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define ZERO_MEM(a) memset(a, 0, sizeof(a))

using namespace std;

class AnimatedModel
{
public:
	AnimatedModel();
	~AnimatedModel();

	bool LoadMesh(const string& Filename);
	
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
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;

		BoneInfo()
		{
			BoneOffset = glm::mat4(0.0f);
			FinalTransformation = glm::mat4(0.0f);
		}
	};

	struct VertexBoneData
	{
		unsigned int IDs[NUM_BONES_PER_VEREX];
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

		void AddBoneData(unsigned int BoneID, float Weight);
	};

	//Calculate interpolations functions
	//other animation functions...
	bool InitFromScene(const aiScene* pScene, const string& Filename);
	bool InitMaterials(const aiScene* pScene, const string& Filename);
	void InitMesh(unsigned int MeshIndex,
		const aiMesh* paiMesh,
		vector<glm::vec3>& Positions,
		vector<glm::vec3>& Normals,
		vector<glm::vec2>& TexCoords,
		vector<VertexBoneData>& Bones,
		vector<unsigned int>& Indices);
	void LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones);

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
	vector<SATexture*> m_Textures;

	map<string, unsigned int> m_BoneMapping; // maps a bone name to its index
	unsigned int m_NumBones;
	vector<BoneInfo> m_BoneInfo;
	aiMatrix4x4 m_GlobalInverseTransform; // TODO something about this - it should be a glm::mat4
	
	glm::mat4 glm_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));
	
	aiMatrix4x4 myAssMatrix;
	glm::mat4 matfromass;

	const aiScene* m_pScene;
	Assimp::Importer m_Importer;
};
