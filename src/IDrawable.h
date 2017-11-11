#pragma once

#include "Shader.h"

class IDrawable
{
public:
	virtual void Draw(Shader* pShader) = 0;
};

