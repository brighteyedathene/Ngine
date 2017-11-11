#pragma once

#include <glm/glm.hpp>

#include "Transform.h"
#include "Shader.h"
#include "IDrawable.h"

class NaiveGameObject
{
public:
	Transform transform;
	IDrawable* m_pMesh;

	NaiveGameObject();
	~NaiveGameObject();

	virtual void Tick();
	virtual void Draw(Shader* pShader);


	inline glm::mat4 GetOffsetTransformMatrix()
	{
		Transform offset(transform);
		offset.rotation.y = -offset.rotation.y + 90.0f;
		offset.rotation.x = -offset.rotation.x;
		
		return offset.GetMatrix();
	}
};

