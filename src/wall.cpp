#include "wall.h"

wall_t::wall_t() : gpu_obj_t() {
	data = nullptr;
	init();
}

wall_t::wall_t(GLfloat* input_data) : gpu_obj_t(input_data) {
	init();
}

void wall_t::init() {
	this->type = GL_QUADS;

	this->vao = new VAO;

	this->vao->element_amount = 5;

	this->data_block_size = { 3, 3, 2 };

	this->data = new float[] {
		//vertices

		10.0f, 15.0f, -10.0f,  //the first wall
		-10.0f, 15.0f, -10.0f,
		-10.0f, -0.5f, -10.0f,
		10.0f, -0.5f, -10.0f,

		-10.0f, 15.0f, 10.0f,  //the second wall
		-10.0f, 15.0f, -10.0f,
		-10.0f, -0.5f, -10.0f,
		-10.0f, -0.5f, 10.0f,

		10.0f, -0.5f, 10.0f,  //floor
		10.0f, -0.5f, -10.0f,
		-10.0f, -0.5f, -10.0f,
		-10.0f, -0.5f, 10.0f,

		10.0f, 15.0f, 10.0f,  //the third wall
		10.0f, 15.0f, -10.0f,
		10.0f, -0.5f, -10.0f,
		10.0f, -0.5f, 10.0f,

		10.0f, 15.0f, 10.0f,  //the forth wall
		-10.0f, 15.0f, 10.0f,
		-10.0f, -0.5f, 10.0f,
		10.0f, -0.5f, 10.0f,

		// normal

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		//texture coordinate

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

	};

	element = new GLuint[]{
		0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19
	};
	
	//GLuint element[12];
	//for (int i = 0; i < 12; ++i) {element[i] = i;}
}

void wall_t::bind() {
	if (this->shader) return;

	this->shader = new
		Shader(
			PROJECT_DIR "/src/shaders/simple.vert",
			nullptr, nullptr, nullptr,
			PROJECT_DIR "/src/shaders/simple.frag");

	this->texture = new
		Texture2D(PROJECT_DIR "/Images/tiles.jpg");
	this->texture->bind(0);
	glUniform1i(glGetUniformLocation(this->shader->Program, "u_texture"), 0);

	glGenVertexArrays(1, &this->vao->vao);
	glGenBuffers(3, this->vao->vbo);
	glGenBuffers(1, &this->vao->ebo);

	glBindVertexArray(this->vao->vao);



	glBindBuffer(GL_ARRAY_BUFFER, this->vao->vbo[0]); //vertice
	//glBufferData(GL_ARRAY_BUFFER, sizeof(wall_vertices), wall_vertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, this->vao->element_amount * 8 * 4, data, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0*sizeof(GLfloat), (void*)0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0*sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->vao->vbo[1]); //normal
	//glBufferData(GL_ARRAY_BUFFER, sizeof(wall_color), wall_color, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, this->vao->element_amount * 8 * 4, data, GL_STATIC_DRAW);
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0*sizeof(GLfloat), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0*sizeof(GLfloat), (void*)(this->vao->element_amount * 4 * data_block_size[0]));
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, this->vao->vbo[2]); //texture
	//glBufferData(GL_ARRAY_BUFFER, sizeof(wall_texture_coordinate), wall_texture_coordinate, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, this->vao->element_amount * 8 * 4, data, GL_STATIC_DRAW);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0* sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0* sizeof(GLfloat), (GLvoid*)(this->vao->element_amount * ( 4 * data_block_size[0] + 3 * data_block_size[1])));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vao->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->vao->element_amount * 4, element, GL_STATIC_DRAW);

	glBindVertexArray(0);

	//todo : texture
}

