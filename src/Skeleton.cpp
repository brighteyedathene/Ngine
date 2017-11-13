#include "Skeleton.h"

glm::mat4 Skeleton::GetModelSpaceBindMatrix(int jointIndex)
{
	glm::mat4 mod = m_joints[jointIndex].m_localBindTransform;
	while (m_joints[jointIndex].m_parentIndex != -1)
	{
		int parentIndex = m_joints[jointIndex].m_parentIndex;
		mod = m_joints[parentIndex].m_localBindTransform * mod;
		jointIndex = parentIndex;
	}
	return mod;
}


void Skeleton::AddAnimationsFromFile(const string& filename)
{
	Assimp::Importer importer;
	const aiScene* pScene;
	pScene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!pScene)
	{
		std::cout << "couldn't load scene from " << filename << std::endl;
		return;
	}
	AddAnimationsFromScene(pScene);
}


void Skeleton::AddAnimationsFromScene(const aiScene* pScene)
{
	for (int i = 0; i < pScene->mNumAnimations; i++)
	{
		aiAnimation* anim = pScene->mAnimations[i];

		Animation newAnim;
		newAnim.Initialize(m_joints.size(), anim->mChannels[0]->mNumPositionKeys, anim->mDuration);
		// TODO find a better way to get the number of keyframes!!!

		for (int c = 0; c < anim->mNumChannels; c++)
		{
			aiNodeAnim* channel;
			channel = anim->mChannels[c];

			if (m_jointMap.find(channel->mNodeName.C_Str()) == m_jointMap.end())
			{
				std::cout << "X  " << c << "couldn't find this bone in mapping: " << channel->mNodeName.C_Str() << std::endl;
				continue;
			}
			int jointIndex = m_jointMap[channel->mNodeName.C_Str()];

			// Mark this joint as used - this means it will use transforms from this animation
			newAnim.jointsUsed[jointIndex] = true;

			for (int k = 0; k < channel->mNumPositionKeys; k++)
			{
				glm::vec3 trans = glm::vec3(channel->mPositionKeys[k].mValue.x,
											channel->mPositionKeys[k].mValue.y,
											channel->mPositionKeys[k].mValue.z);
				glm::quat rotation = glm::quat(channel->mRotationKeys[k].mValue.w,
											   channel->mRotationKeys[k].mValue.x,
											   channel->mRotationKeys[k].mValue.y,
											   channel->mRotationKeys[k].mValue.z);
				glm::vec3 scale = glm::vec3(channel->mScalingKeys[k].mValue.x,
											channel->mScalingKeys[k].mValue.y,
											channel->mScalingKeys[k].mValue.z);

				newAnim.keyframes[k].jointPoses[jointIndex].position = trans;
				newAnim.keyframes[k].jointPoses[jointIndex].rotation = rotation;
				newAnim.keyframes[k].jointPoses[jointIndex].scale = scale;

				// Not robust! if any channels have different times then this breaks everything
				newAnim.keyframes[k].timestamp = channel->mPositionKeys[k].mTime;
			}
		}
		m_animations.push_back(newAnim);
		m_animMap[anim->mName.C_Str()] = m_animations.size() - 1;
	}
}