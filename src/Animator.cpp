#include "Animator.h"

#include "AssortedUtils.h"

Animator::Animator(AnimatedModel* pModel)
{
	m_pModel = pModel;
	m_jointCount = m_pModel->m_Skeleton.m_joints.size();
	m_currentMatrices = new glm::mat4[m_jointCount];
	m_modelSpaceTransforms = new glm::mat4[m_jointCount];
	m_currentTime = 0;
}


Animator::~Animator()
{
	delete m_currentMatrices;
}

//void Animator::PlayAnimation(string name, maybe enum, bool loop, speed?);

void Animator::Tick(float deltaTime)
{
	m_currentTime += deltaTime * 3.0f;
	
	GetNextPose();


}

void Animator::GetNextPose()
{
	// current animation
	Animation anim = m_pModel->m_Skeleton.m_animations[0];
	anim.loop = true;
	m_currentTime = fmodf(m_currentTime, anim.duration);

	// Get closest 2 keyframes from animation
	// TODO improve this mess
	int prev = 0;
	int next = 1;
	if (anim.keyframes.size() < 2)
	{
		std::cout << "one frame animation - should do something about this" << std::endl;
		next = 0;
	}
	else
	{
		while (prev < anim.keyframes.size() - 1)
		{
			if (m_currentTime < anim.keyframes[prev+1].timestamp)
				break;

			prev++;
		}
		next = prev + 1;
		if (anim.loop)
			next = next % anim.keyframes.size();
		
	}

	// Interploate between 2 keyframes across all joint poses
	float difference = anim.keyframes[next].timestamp - anim.keyframes[prev].timestamp;
	float t = m_currentTime - anim.keyframes[prev].timestamp;
	float alpha = t / difference;
	
	for (int i = 0; i < m_jointCount; i++)
	{
		glm::mat4 jointTransform; // the local transform for this joint
		if (anim.jointsUsed[i])
		{
			glm::vec3 interpos = anim.keyframes[prev].jointPoses[i].position * (1 - alpha)
								 + anim.keyframes[next].jointPoses[i].position * alpha;

			glm::vec3 interscale = anim.keyframes[prev].jointPoses[i].scale * (1 - alpha)
								   + anim.keyframes[next].jointPoses[i].scale * alpha;

			glm::quat interrot = glm::lerp(anim.keyframes[prev].jointPoses[i].rotation,
										   anim.keyframes[next].jointPoses[i].rotation,
										   alpha);

			glm::mat4 transMat = glm::translate(glm::mat4(1.0f), interpos);
			glm::mat4 rotationMat = glm::mat4_cast(interrot);
			glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), interscale);

			jointTransform = transMat * rotationMat * scaleMat;
		}
		else // this joint isn't included in the animation, so it's just in bind pose
		{
			jointTransform = m_pModel->m_Skeleton.m_joints[i].m_localBindTransform;
		}

		glm::mat4 parentMat;
		int parentIndex = m_pModel->m_Skeleton.m_joints[i].m_parentIndex;
		if (parentIndex >= 0)
			parentMat = m_modelSpaceTransforms[parentIndex];
		else
			parentMat = glm::mat4(1.0f);

		m_modelSpaceTransforms[i] = parentMat * jointTransform;

		glm::mat4 finalTransform = m_pModel->m_Skeleton.m_globalInverseBindTransform
			* m_modelSpaceTransforms[i]
			* m_pModel->m_Skeleton.m_joints[i].m_modelBindTransform;

		m_currentMatrices[i] = finalTransform;
	}
}