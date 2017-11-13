#pragma once

#include <vector>
#include <map>
#include <iostream>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Animation.h"

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