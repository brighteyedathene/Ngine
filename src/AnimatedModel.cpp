#include "AnimatedModel.h"

#include "AssortedUtils.h"
#include <limits>

#define POSITION_LOCATION    0
#define NORMAL_LOCATION      1
#define TEX_COORD_LOCATION   2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4





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

		if (minWeight > 0)
		{
			std::cout << "replaced a non-zero skin weight for joint " << BoneID << std::endl;
		}
	}
}


void AnimatedModel::NormalizeSkinWeights(vector<VertexBoneData>& SkinWeights)
{
	for (unsigned int i = 0; i < SkinWeights.size(); i++)
	{
		float sum = 0;
		for (int j = 0; j < NUM_BONES_PER_VEREX; j++)
		{
			sum += SkinWeights[i].Weights[j];
		}
		if (sum == 0)
		{
			std::cout << "PROBLEM: a supposedly animated vertex has no joints!" << std::endl;
			continue;
		}
		else if (sum == 1)
		{
			// everything is fine - weigths add up to one
			continue;
		}
		//std::cout << "before " << SkinWeights[i].IDs[0] << ": " << SkinWeights[i].Weights[0] << ", "
		//	<< SkinWeights[i].IDs[1] << ": " << SkinWeights[i].Weights[1] << ", "
		//	<< SkinWeights[i].IDs[2] << ": " << SkinWeights[i].Weights[2] << ", "
		//	<< SkinWeights[i].IDs[3] << ": " << SkinWeights[i].Weights[3] << "     " << sum << std::endl;

		float normalizer = 1 / sum;
		float nusum = 0;
		for (int j = 0; j < NUM_BONES_PER_VEREX; j++)
		{
			SkinWeights[i].Weights[j] *= normalizer;
			nusum += SkinWeights[i].Weights[j];
		}
		//std::cout << "after  " << SkinWeights[i].IDs[0] << ": " << SkinWeights[i].Weights[0] << ", "
		//	<< SkinWeights[i].IDs[1] << ": " << SkinWeights[i].Weights[1] << ", "
		//	<< SkinWeights[i].IDs[2] << ": " << SkinWeights[i].Weights[2] << ", "
		//	<< SkinWeights[i].IDs[3] << ": " << SkinWeights[i].Weights[3] << "     " << nusum << std::endl;
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
	glBindVertexArray(m_VAO);
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	bool ret = false;

	m_pScene = m_Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

	if (m_pScene)
	{
		aiMatrix4x4 globalInverseTransform = m_pScene->mRootNode->mTransformation;
		globalInverseTransform.Inverse();
		//m_GlobalInverseTransform = GetGlmMat4FromAssimp(globalInverseTransform);
		m_Skeleton.m_globalInverseBindTransform = GetGlmMat4FromAssimp(globalInverseTransform);
		ret = InitFromScene(m_pScene, Filename);
	}
	else
	{
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
	}

	glBindVertexArray(0);
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

	//NormalizeSkinWeights(SkinWeights);

	if (!InitMaterials(pScene, Filename))
	{
		std::cout << "No materials for animated models yet!" << std::endl;
	}

	std::cout << "what do with these animations?" << std::endl;



	//recursive_print(pScene->mRootNode, 0);
	//LoadAnimations(pScene);
	m_Skeleton.AddAnimationsFromFile(Filename);


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
	glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

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

// TODO split this into LoadSkeleton and LoadSkin
// assemeble skin and bones
void AnimatedModel::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& SkinWeights)
{
	aiNode* pRoot = m_pScene->mRootNode;

	for (unsigned int i = 0; i < pMesh->mNumBones; i++)
	{
		unsigned int BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);

		if (m_Skeleton.m_jointMap.find(BoneName) == m_Skeleton.m_jointMap.end())
		{
			// Add a new Joint
			Joint joint;

			// Attempt to find this bone's parent in the node hierarchy
			aiNode* pNode = pRoot->aiNode::FindNode(BoneName.c_str());
			if (!pNode)
			{
				std::cout << "Couldn't find a node with name: " << BoneName.c_str() << " in node hierarchy!" << std::endl;
			}

			// Now try to find the node's parent
			// if it hasn't been mapped yet, assume this node is a root and give it no parent
			if (pNode->mParent == NULL) {
				joint.m_parentIndex = -1;
			}
			else {
				string parentName(pNode->mParent->mName.data);
				if (m_Skeleton.m_jointMap.find(parentName) == m_Skeleton.m_jointMap.end())
				{
					std::cout << "This node was referenced as a parent bone without being mapped: " << parentName << std::endl;
					std::cout << "----Assuming the following node is a root: " << BoneName.c_str() << std::endl;
					joint.m_parentIndex = -1;
				}
				else
				{
					joint.m_parentIndex = m_Skeleton.m_jointMap[parentName];
				}
			}


			// Set the joint ID
			joint.m_index = m_NumBones;

			// Set the model-space and local-space transforms
			glm::mat4 offsetTransform = GetGlmMat4FromAssimp(pMesh->mBones[i]->mOffsetMatrix);
			glm::mat4 localTransform = GetGlmMat4FromAssimp(pNode->mTransformation);
			
			joint.m_localBindTransform = localTransform;
			joint.m_modelBindTransform = offsetTransform;

			m_Skeleton.m_joints.push_back(joint);
			if (joint.m_index != m_Skeleton.m_joints.size()-1)
			{
				std::cout << "jointid mismatch - m_numBones gave this joint the wrong id " << joint.m_name << std::endl;
				std::cout << "---- m_numBones: " << joint.m_index << "   actual id:: " << m_Skeleton.m_joints.size() - 1 << std::endl;
			}

			// X. This is the old stuff
			// Allocate an index for anew bone
			BoneIndex = m_NumBones;
			m_NumBones++;
	
			m_Skeleton.m_jointMap[BoneName] = BoneIndex;
			m_Skeleton.m_idMap[BoneIndex] = BoneName;
		}
		else
		{
			// Just use the existing joint
			BoneIndex = m_Skeleton.m_jointMap[BoneName];
		}

		// fucking dump all this bone's influences into the vertex-bone-weights blob for this mesh
		for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++)
		{
			unsigned int VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			SkinWeights[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}

	// This line shrinks the vector to fit 
	// see https://stackoverflow.com/questions/253157/how-to-downsize-stdvector
	// This can cause a reallocation if the meshes contian different bones
	// - but i'd rather leave it here and forget about it
	vector<Joint>(m_Skeleton.m_joints).swap(m_Skeleton.m_joints);
}



bool AnimatedModel::InitMaterials(const aiScene* pScene, const string& Filename)
{
	// Extract the directory part from the file name
	string::size_type SlashIndex = Filename.find_last_of("/");
	string dir;


	if (SlashIndex == string::npos)
		dir = ".";
	else if (SlashIndex == 0)
		dir = "/";
	else
		dir = Filename.substr(0, SlashIndex);

	bool success = true;

	for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
	{
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		m_Textures[i] = NULL;

		//cout << dir << ":" << endl;
		//for (int j = 0; j < pMaterial->mNumProperties; j++)
		//{
		//	cout << pMaterial->mProperties[j]->mType << " " << pMaterial->mProperties[j]->mKey.C_Str() << endl;
		//}

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				string p(path.data);

				if (p.substr(0, 2) == ".\\") // Don't know what's happening here really
					p = p.substr(2, p.size() - 2);

				m_Textures[i] = new Texture;
				if (m_Textures[i]->LoadFromFile(p.c_str(), dir) == false)
					success = false;

			}
		}
		else
		{
			cout << "no diffuse textures found for material #" << i << endl;
		}
		if (pMaterial->GetTextureCount(aiTextureType_UNKNOWN) > 0)
		{
			cout << "unknown texture type detected!" << endl;
		}
		
	}
	return success;
}


void AnimatedModel::Draw(Shader* pShader)
{

	// TODO set the mesh's materials here
	pShader->SetVec3("material.ambient", 1, 1, 1);
	pShader->SetVec3("material.diffuse", 0.6, 0.6, 0.6);
	pShader->SetVec3("material.specular", 1, 1, 1);
	pShader->SetFloat("material.shininess", 2.0f);
	pShader->SetFloat("material.shininess", 3.0f);

	glBindVertexArray(m_VAO);
	for (MeshEntry mesh : m_Entries)
	{
		//TODO bind materials
		if (m_Textures[mesh.MaterialIndex])
		{
			pShader->SetInt("material.diffuseTexture", 0);
			m_Textures[mesh.MaterialIndex]->Bind(GL_TEXTURE0);
		}
		

		glDrawElementsBaseVertex(GL_TRIANGLES, 
								 mesh.NumIndices,
								 GL_UNSIGNED_INT, 
								 (void*)(sizeof(unsigned int) * mesh.BaseIndex), 
								 mesh.BaseVertex);
	}
	glBindVertexArray(0);
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
