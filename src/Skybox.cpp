#include "Skybox.h"



Skybox::Skybox(vector<std::string> faces)
{
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox::skyboxVertices), &skybox::skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	m_cubemap.LoadCubemap(faces);
}


Skybox::~Skybox()
{
}

void Skybox::Draw(Shader* pShader, Camera* pCamera)
{
	glDepthFunc(GL_LEQUAL);
	pShader->Use();
	pShader->SetInt("skybox", 0);

	glm::mat4 view = glm::mat4(glm::mat3(pCamera->GetViewMatrix()));
	glm::mat4 projectionview = pCamera->GetProjectionMatrix() * view;
	pShader->SetMat4("projectionview", projectionview);

	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap.m_textureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);

}