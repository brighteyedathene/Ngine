#include "AnimatedModel.h"

#include "AssortedUtils.h"
#include <limits>

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

map<unsigned int, bool> bones_used;

void AnimatedModel::VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
	unsigned int minIndex;
	float minWeight = std::numeric_limits<float>::infinity();

	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++)
	{
		if (Weights[i] < minWeight)
		{
			minIndex = i;
			minWeight = Weights[i];
		}
	}

	if (Weight > minWeight)
	{
		IDs[minIndex] = BoneID;
		Weights[minIndex] = Weight;
	}
}

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

bool AnimatedModel::LoadMesh(const string& Filename)
{
	// Release previously loaded mesh
	Clear();

	// Create the VAO
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	bool ret = false;

	m_pScene = m_Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

	if (m_pScene)
	{
		// TODO glm-ify this matrix
		m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
		m_GlobalInverseTransform.Inverse();
		ret = InitFromScene(m_pScene, Filename);
	}
	else
	{
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
	}

	return ret;
}


string chop_before(aiString aiStr, char delimiter)
{
	string s = "";
	for (int i = aiStr.length - 1; i >= 0; i--)
	{
		if (aiStr.data[i] == delimiter)
		{
			for (int j = 1; j < aiStr.length - i; j++)
			{
				s += aiStr.data[i + j];
			}
			return s;
		}
	}
	return "couldn't find delimiter";
}

void print_animations(const aiScene* pScene)
{
	for (int i = 0; i < pScene->mNumAnimations; i++)
	{
		//aiAnimation* anim = &(pScene->mAnimations[i]);
		for (int c = 0; c < pScene->mAnimations[i]->mNumChannels; c++)
		{
			std::cout << "channel: " << pScene->mAnimations[i]->mChannels[c]->mNodeName.C_Str() << std::endl;
			aiNodeAnim* channel;
			for (int k = 0; k < pScene->mAnimations[i]->mChannels[c]->mNumPositionKeys; k++)
			{
				channel = pScene->mAnimations[i]->mChannels[c];
				std::cout << "key: " << k << "               time " << pScene->mAnimations[i]->mChannels[c]->mScalingKeys[k].mTime
					<< std::endl << "pos:   "
					<< "  x: " << pScene->mAnimations[i]->mChannels[c]->mPositionKeys[k].mValue.x
					<< "  y: " << pScene->mAnimations[i]->mChannels[c]->mPositionKeys[k].mValue.y
					<< "  z: " << pScene->mAnimations[i]->mChannels[c]->mPositionKeys[k].mValue.z
					<< std::endl << "rot:   "
					<< "  z: " << pScene->mAnimations[i]->mChannels[c]->mRotationKeys[k].mValue.w
					<< "  x: " << pScene->mAnimations[i]->mChannels[c]->mRotationKeys[k].mValue.x
					<< "  y: " << pScene->mAnimations[i]->mChannels[c]->mRotationKeys[k].mValue.y
					<< "  z: " << pScene->mAnimations[i]->mChannels[c]->mRotationKeys[k].mValue.z
					<< std::endl << "scale: "
					<< "  x: " << pScene->mAnimations[i]->mChannels[c]->mScalingKeys[k].mValue.x
					<< "  y: " << pScene->mAnimations[i]->mChannels[c]->mScalingKeys[k].mValue.y
					<< "  z: " << pScene->mAnimations[i]->mChannels[c]->mScalingKeys[k].mValue.z
					<< std::endl;
			}
		}
	}
}

void recursive_print(aiNode* node, int depth)
{
	string tab = "- ";
	for (int i = 0; i < depth; i++)
		tab += "- ";

	//string name = chop_before(node->mName, '_');
	//std::cout << tab << name << std::endl;
	std::cout << tab << node->mName.C_Str() << std::endl;

	for (int i = 0; i < node->mNumChildren; i++)
		recursive_print(node->mChildren[i], depth + 1);
}

bool AnimatedModel::InitFromScene(const aiScene* pScene, const string& Filename)
{
	m_Entries.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);

	vector<glm::vec3> Positions;
	vector<glm::vec3> Normals;
	vector<glm::vec2> TexCoords;
	vector<VertexBoneData> SkinWeights;
	vector<unsigned int> Indices;

	// TODO finish this function (ogldev_skinned_mesh.cpp)
	unsigned int NumVertices = 0;
	unsigned int NumIndices = 0;
	
	// Count vertices and indices
	for (unsigned int i = 0; i < m_Entries.size(); i++)
	{
		m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Entries[i].BaseVertex = NumVertices;
		m_Entries[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[i].NumIndices;
	}

	// Reserve space in the vectors for vertex attributes and indices
	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Indices.reserve(NumVertices);

	SkinWeights.resize(NumVertices); // Resize!!

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < m_Entries.size(); i++)
	{
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, Positions, Normals, TexCoords, SkinWeights, Indices);
	}

	if (!InitMaterials(pScene, Filename))
	{
		std::cout << "No materials for animated models yet!" << std::endl;
	}

	std::cout << "what do with these animations?" << std::endl;
	// TODO finish this function

	// here's some recon stuff

	//recursive_print(pScene->mRootNode, 0);
	//print_animations(pScene);



	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SkinWeights) * SkinWeights.size(), &SkinWeights[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribPointer(BONE_ID_LOCATION, 4, GL_INT, GL_FALSE, sizeof(VertexBoneData), (void*)0);
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	return false;
}


void AnimatedModel::InitMesh(unsigned int MeshIndex,
	const aiMesh* paiMesh,
	vector<glm::vec3>& Positions,
	vector<glm::vec3>& Normals,
	vector<glm::vec2>& TexCoords,
	vector<VertexBoneData>& SkinWeights,
	vector<unsigned int>& Indices)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	// Populate the vertex attribute vectors
	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
	{
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
		Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
		TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
	}


	LoadBones(MeshIndex, paiMesh, SkinWeights);
	// TODO normalise SkinWeights

	// Populate the index buffer
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
	{
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}
}


// assemeble skin and bones
void AnimatedModel::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& SkinWeights)
{
	m_Skeleton.m_jointCount = pMesh->mNumBones;
	m_Skeleton.m_joints.resize(m_Skeleton.m_jointCount);
	aiNode* pRoot = m_pScene->mRootNode;

	for (unsigned int i = 0; i < pMesh->mNumBones; i++)
	{
		unsigned int BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
		{

			// Joints
			Joint joint;
			// 1.
			// Attempt to find this bone's parent in the node hierarchy
			aiNode* pNode = pRoot->aiNode::FindNode(BoneName.c_str());
			if (!pNode)
			{
				std::cout << "Couldn't find a node with name: " << BoneName.c_str() << std::endl;
			}
			//2.
			// Now try to find the node's parent
			// if it hasn't been mapped yet, assume this node is a root and give it no parent
			string parentName(pNode->mParent->mName.data);
			if (m_BoneMapping.find(parentName) == m_BoneMapping.end())
			{
				std::cout << "This node was referenced as a parent bone without being mapped: " << parentName << std::endl;
				std::cout << "----Assuming the following node is a root: " << BoneName.c_str() << std::endl;
				joint.m_parentIndex = -1;
			}
			else
			{
				joint.m_parentIndex = m_BoneMapping[parentName];
			}

			// 3.
			// Set the joint ID
			joint.m_index = m_NumBones;
			//4.
			// Set the model-space Bind transform (maybe)
			aiMatrix4x4 aiBindTransform = pMesh->mBones[i]->mOffsetMatrix;
			glm::mat4 bindTransform; 
			AssToGlmMat4(aiBindTransform, bindTransform);

			glm::mat4 localTransform;
			AssToGlmMat4(pNode->mTransformation, localTransform);
			joint.m_localBindTransform = localTransform;

			//if (joint.m_parentIndex >= 0)
			//	bindTransform = m_Skeleton.m_joints[joint.m_parentIndex].m_modelBindTransform * bindTransform;
			
			joint.m_modelBindTransform = bindTransform;

			//5.
			// Get the inverse Bind transform
			joint.m_inverseBindTransform = glm::affineInverse(joint.m_modelBindTransform);



			//std::cout << BoneName.c_str() << std::endl;
			//print_matrix(" model", joint.m_modelBindTransform);
			//print_matrix(" inv", joint.m_inverseBindTransform);
			//print_matrix("identity verification:", joint.m_inverseBindTransform * joint.m_modelBindTransform);

			m_Skeleton.m_joints[joint.m_index] = joint;

			// X. This is the old stuff
			// Allocate an index for anew bone
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			aiMatrix4x4 offset = pMesh->mBones[i]->mOffsetMatrix;
			AssToGlmMat4(pMesh->mBones[i]->mOffsetMatrix, m_BoneInfo[BoneIndex].BoneOffset);
			//m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix; // This was the original, un-glm'd line
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else
		{
			BoneIndex = m_BoneMapping[BoneName];
		}

		// fucking dump all this bone's influences into the vertex-bone-weights blob
		for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++)
		{
			unsigned int VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			SkinWeights[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}

void AnimatedModel::AssembleSkeleton(const aiNode* pRootNode, 
									vector<VertexBoneData>& SkinWeights, 
									Skeleton* m_skeleton)
{

}

bool AnimatedModel::InitMaterials(const aiScene* pScene, const string& Filename)
{
	// TODO write this function
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
