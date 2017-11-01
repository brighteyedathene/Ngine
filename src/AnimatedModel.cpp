#include "AnimatedModel.h"


#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4


AnimatedModel::AnimatedModel()
{
	m_VAO = 0;
	ZERO_MEM(m_Buffers);
	m_NumBones = 0;
	m_pScene = NULL;
}


AnimatedModel::~AnimatedModel()
{
}

bool AnimatedModel::LoadMesh(const string& filename)
{
	// Release previously loaded mesh
	Clear();

	// Create the VAO
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	bool ret = false;

	m_pScene = m_Importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

	if (m_pScene)
	{
		m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
		m_GlobalInverseTransform.Inverse();
		ret = InitFromScene(m_pScene, filename);
	}
	else
	{
		printf("Error parsing '%s': '%s'\n", filename.c_str(), m_Importer.GetErrorString());
	}

	myAssMatrix = aiMatrix4x4();
	myAssMatrix = myAssMatrix.Translation(aiVector3D(1.0f, 2.0f, 3.0f), myAssMatrix);

	// copies the elements from assimp's aiMatrix4x4 to glm's mat4
	Assorted::AssToGlmMat4(myAssMatrix, matfromass);

	Assorted::print_matrix("ass", matfromass);
	Assorted::print_matrix("raw glm", glm_matrix);

	return ret;
}


bool AnimatedModel::InitFromScene(const aiScene* pScene, const string* filename)
{
	m_Entries.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);

	vector<glm::vec3> Positions;
	vector<glm::vec3> Normals;
	vector<glm::vec2> TexCoords;
	vector<VertexBoneData> Bones;
	vector<unsigned int> Indices;

	// TODO finish this function (ogldev_skinned_mesh.cpp)

	return false;
}

void AnimatedModel::Render()
{
	// TODO
}

void AnimatedModel::BoneTransform(float timeInSeconds, vector<glm::mat4>& transforms)
{
	// TODO
}

void AnimatedModel::Clear()
{
	//for (unsigned int i = 0; i < m_Textures.size(); i++)
	//{
	//	SAFE_DELETE(m_Textures[i]);
	//}

	if (m_Buffers[0] != 0)
	{
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
	}

	if (m_VAO != 0)
	{
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}
