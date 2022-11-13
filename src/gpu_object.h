#ifndef GPU_OBJECT_H
#define GPU_OBJECT_H
#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glu.h>
#include "RenderUtilities/BufferObject.h"
#include "RenderUtilities/Shader.h"
#include "RenderUtilities/Texture.h"
#include "Utilities/Pnt3f.H"

class gpu_object {
private:
protected:
	Shader* shader		= nullptr;
	Texture2D* texture	= nullptr;
	VAO* data			= nullptr;
	UBO* commom_matrices= nullptr;
	virtual void init();
	
	virtual void GL_Draw() = 0;
public:
	void Draw();
	virtual void GL_Init();
};

#endif