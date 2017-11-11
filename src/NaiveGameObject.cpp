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
	pShader->SetMat4("model", GetOffsetTransformMatrix());
	m_pMesh->Draw(pShader);
}