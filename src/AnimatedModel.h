#pragma once

#include <map>
#include <vector>
#include <string>
#include <cassert>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "SATexture.h"
#include "IDrawable.h"
#include "Shader.h" // to conform to IDrawable class :( what am i doing?!


// some handy shit from ogldev's thing
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define ZERO_MEM(a) memset(a, 0, sizeof(a))

using namespace std;

struct Animation;

struct Joint
{
	// (ModelSpaceParentBindPose * LocalBindPose).inverse()
	glm::mat4 m_localBindTransform; // transform relative to parent joint
	glm::mat4 m_modelBindTransform; // sends a vertex from model space to local space
	const char* m_name;
	int m_index;
	int m_parentIndex;
	
	vector<Joint> children;
};

struct Skeleton
{
	int m_jointCount;
	vector<Joint> m_joints;
	map<string, unsigned int> m_jointMap; // maps a joint name to its index

	map<int, string> m_idMap;

	glm::mat4 m_globalInverseBindTransform;

	vector<Animation> m_animations;
	map<string, unsigned int> m_animMap;


	glm::mat4 GetModelSpaceBindMatrix(int jointIndex);
	void AddAnimationsFromFile(const string& filename);
	void AddAnimationsFromScene(const aiScene* pScene);
};

struct JointPose
{
	glm::quat rotation;
	glm::vec3 position;
	glm::vec3 scale;

	JointPose()
	{
		rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		position = glm::vec3(0.0f);
		scale = glm::vec3(1.0f);
	}
};

struct Keyframe
{
	vector<JointPose> jointPoses;
	float timestamp;
};

struct Animation
{
	vector<Keyframe> keyframes;
	vector<bool> jointsUsed;
	float duration;
	bool loop;

	void Initialize(int numJoints, int numKeyframes, float duration);
};

class AnimatedModel : public IDrawable
{
public:
	AnimatedModel();
	~AnimatedModel();

	bool LoadMesh(const string& Filename);
	void Draw(Shader* pShader);
	void Clear();
	
	Skeleton m_Skeleton;


private:

#define NUM_BONES_PER_VEREX 4

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
	void NormalizeSkinWeights(vector<VertexBoneData>& SkinWeights);

	//other animation functions...
	bool InitFromScene(const aiScene* pScene, const string& Filename);
	bool InitMaterials(const aiScene* pScene, const string& Filename);
	void InitMesh(unsigned int MeshIndex,
		const aiMesh* paiMesh,
		vector<glm::vec3>& Positions,
		vector<glm::vec3>& Normals,
		vector<glm::vec2>& TexCoords,
		vector<VertexBoneData>& SkinWeights,
		vector<unsigned int>& Indices);
	void LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& SkinWeights);
	

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

	glm::mat4 m_GlobalInverseTransform;

	const aiScene* m_pScene;
	Assimp::Importer m_Importer;
};
