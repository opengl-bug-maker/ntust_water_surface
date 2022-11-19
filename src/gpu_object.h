#pragma once

//#ifdef GPU_OBJECT

#include "RenderUtilities/Shader.h"
#include "RenderUtilities/Texture.h"
#include "RenderUtilities/BufferObject.h"
#include "Utilities/3DUtils.h"
#include "glm/mat4x4.hpp"

class gpu_obj_t {
protected:
	Texture2D* texture = nullptr; //init
	skybox_t* skybox = nullptr; //init
	VAO* vao = nullptr; //mutiple vbos and 1 ebo

	GLfloat* data = nullptr; //init
	GLuint* element = nullptr; //init
	GLenum type = GL_TRIANGLES; //init
	//GL_TRIANGLES
	//GL_QUADS

	glm::mat4 model_matrix; //for scaling, ratation, translation
	
	vector<size_t> data_block_size; //init
	vector<gpu_obj_t*> children; //init

	virtual void init();

	void update(float* input_data, size_t index);

public:
	virtual void bind() = 0;
	Shader* shader = nullptr; //init
	gpu_obj_t();
	explicit gpu_obj_t(GLfloat* input_data);
	explicit gpu_obj_t(glm::mat4 model_matrix);
	explicit gpu_obj_t(GLfloat* input_data, glm::mat4 model_matrix);

	void draw(glm::mat4 modelMatrix = glm::mat4(1));

};