#pragma once

#include <glm/glm.hpp>

#include "Transform.h"
#include "Shader.h"
#include "Material.h"
#include "Input.h"
#include "IDrawable.h"

class NaiveGameObject
{
public:
	Transform transform;
	Material material;

	NaiveGameObject();
	~NaiveGameObject();

	virtual void Tick();
	virtual void Draw(Shader* pShader);


	inline void SetMesh(IDrawable* pMesh) { m_pMesh = pMesh; }
	inline void SetInput(Input* pInput) { m_pInput = pInput; }

private:
	Input* m_pInput;
	IDrawable* m_pMesh;

	inline glm::mat4 GetOffsetTransformMatrix()
	{
		Transform offset(transform);
		offset.rotation.y = -offset.rotation.y + 90.0f;
		offset.rotation.x = -offset.rotation.x;

		return offset.GetMatrix();
	}
};

