#include "NaiveGameObject.h"



NaiveGameObject::NaiveGameObject()
{
}


NaiveGameObject::~NaiveGameObject()
{
}

void NaiveGameObject::Tick()
{

}

void NaiveGameObject::Draw(Shader* pShader)
{
	pShader->SetVec3("material.ambient", material.ambient);
	pShader->SetVec3("material.diffuse", material.diffuse);
	pShader->SetVec3("material.specular", material.specular);
	pShader->SetFloat("material.shininess", material.shininess);
	pShader->SetMat4("model", GetOffsetTransformMatrix());
	m_pMesh->Draw(pShader);
}