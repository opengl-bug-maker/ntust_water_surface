#include "water.h"
#include "TrainWindow.h"
#include "TrainView.h"
#include "Utilities/ArcBallCam.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
water_t::water_t() {
	init();
}

water_t::water_t(GLfloat* input_data) {
	init();
}

void water_t::init() {
	this->type = GL_TRIANGLES;

	this->vao = new VAO;

	this->vao->element_amount = (count - 1) * (count - 1) * 2; // todo

	this->data_block_size = { 3, 3, 2 };
	
	this->model_matrix = glm::mat4();
	this->model_matrix = glm::scale(this->model_matrix, glm::vec3(10.0f, 10.0f, 10.0f));

	float step = 20.0f / this->count;
	this->surfaces = new surface * [this->count + 2];
	for (int i = 0; i < this->count + 2; ++i) {
		this->surfaces[i] = new surface[this->count + 2];
		for (int j = 0; j < this->count + 2; ++j) {
			this->surfaces[i][j].x = -10.0f + step * (i - 1);
			this->surfaces[i][j].y = -10.0f + step * (j - 1);
		}
	}

	this->data = new GLfloat[
		(count * count * 3) + // position
			(count * count * 3) + // normal
			(count * count * 2)   // coodinate
	]{ 0.0f };

	this->element = new GLuint[this->vao->element_amount * 3] { 0 };

	int index = 0;
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1; j++) {
            this->element[index++] = i * count + j;
            this->element[index++] = (i + 1) * count + (j + 1);
            this->element[index++] = (i + 1) * count + j;

            this->element[index++] = i * count + j;
            this->element[index++] = i * count + (j + 1);
            this->element[index++] = (i + 1) * count + (j + 1);
        }
    }
}

void water_t::bind() {
	if (this->shader) return;

	this->shader = new
		Shader(
			PROJECT_DIR "/src/shaders/water.vert",
			nullptr, nullptr, nullptr,
			PROJECT_DIR "/src/shaders/water.frag");

	



	glGenVertexArrays(1, &this->vao->vao);
	glGenBuffers(3, this->vao->vbo);
	glGenBuffers(1, &this->vao->ebo);

	glBindVertexArray(this->vao->vao);
	
	//=====================
    //float baisHeight = 10.0f;

	

	//position
	//int start;
	//start = 0;
	//
	 //   for (int i = 0; i < count; i++) {
	 //       for (int j = 0; j < count; j++) {
	 //           data[start + (i * count + j) * 3 + 0] = surfaces[i + 1][j + 1].x;
	 //           data[start + (i * count + j) * 3 + 1] = surfaces[i + 1][j + 1].height + baisHeight;
	 //           data[start + (i * count + j) * 3 + 2] = surfaces[i + 1][j + 1].y;
	 //       }
	 //   }
	//=====================
	//position
	glBindBuffer(GL_ARRAY_BUFFER, vao->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, this->count * this->count * (3 + 3 + 2) * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	
	////normal
	glBindBuffer(GL_ARRAY_BUFFER, vao->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, this->count * this->count * (3 + 3 + 2) * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)((count * count * 3) * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	////coordinate
	//glBindBuffer(GL_ARRAY_BUFFER, vao->vbo[2]);
	//glBufferData(GL_ARRAY_BUFFER, this->vao->element_amount * 4 * (3 + 3 + 2) * sizeof(GLfloat), data, GL_STATIC_DRAW);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(5 * 4 * (3 + 3) * sizeof(GLfloat)));
	//glEnableVertexAttribArray(2);
	//// -----------------------------------------------------------------------------------------------
	
	//////Element attribute
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->vao->element_amount * 3 * sizeof(GLuint), element, GL_STATIC_DRAW);

	glBindVertexArray(0);

	////todo : texture
	this->texture = new Texture2D();
	this->texture->set2dTexture(PROJECT_DIR "/Images/tiles.jpg");
	//this->texture = new
	//	Texture2D(PROJECT_DIR "/Images/tiles.jpg");
	this->texture->bind(0);
	this->skybox->bind(1);


	/*uniform vec3 u_color;
	uniform vec3 light_color;
	uniform vec3 lightPos;
	uniform sampler2D u_texture;
	uniform vec3 eye_position;*/
	//uniform
	this->shader->Use();
	glUniform1i(glGetUniformLocation(this->shader->Program, "u_texture"),0);
	glUniform1i(glGetUniformLocation(this->shader->Program, "skybox"), 1);
	glUniform3fv(
		glGetUniformLocation(this->shader->Program, "u_color"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
	glUniform3fv(
		glGetUniformLocation(this->shader->Program, "lightPos"), 1, &glm::vec3(0.0f, 20.0f, 1.0f)[0]);
	//glUniform3fv(
	//	glGetUniformLocation(this->shader->Program, "eye_position"), 1, &glm::vec3(0.0f, 2.0f, 0.0f)[0]);
	glm::vec3 eye = glm::vec3(
		TrainWindow::magic->trainView->arcball.eyeX,
		TrainWindow::magic->trainView->arcball.eyeY,
		TrainWindow::magic->trainView->arcball.eyeZ
	);
	glUniform3fv(
		glGetUniformLocation(this->shader->Program, "eye_position"), 1, &eye[0]);
	glUniform3fv(
		glGetUniformLocation(this->shader->Program, "light_color"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
}



void water_t::tickBack() {
    for (int i = 0; i < count + 2; i++) {
        for (int j = 1; j < count + 2; j++) {
            surfaces[i][j].t2Height = surfaces[i][j].t1Height;
            surfaces[i][j].t1Height = surfaces[i][j].height;
        }
    }
}

void water_t::fresh() {
    for (int i = 1; i <= count; ++i) {
        for (int j = 1; j <= count; ++j) {
            float l = surfaces[i - 1][j].t1Height;
            float r = surfaces[i + 1][j].t1Height;
            float u = surfaces[i][j + 1].t1Height;
            float d = surfaces[i][j - 1].t1Height;

            float tickLength = 1;
			
			float slowdown = 0.7;
			float speed = 0.8;

             //surfaces[i][j].height = (1 + slowdown) * surfaces[i][j].t1Height -
             //    slowdown * surfaces[i][j].t2Height +

             //    0.5f * speed * speed * (
             //        l + r + u + d - 4 * surfaces[i][j].t1Height
             //        ) * tickLength * tickLength;
			 
            //surfaces[i][j].height = (1 + tw->SlowDown->value()) * surfaces[i][j].t1Height -
            //    tw->SlowDown->value() * surfaces[i][j].t2Height +

            //    0.5f * tw->Speed->value() * tw->Speed->value() * (
            //        l + r + u + d - 4 * surfaces[i][j].t1Height
            //        ) * tickLength * tickLength;

            surfaces[i][j].height = (1 + TrainWindow::magic->SlowDown->value()) * surfaces[i][j].t1Height -
                TrainWindow::magic->SlowDown->value() * surfaces[i][j].t2Height +

                0.5f * TrainWindow::magic->Speed->value() * TrainWindow::magic->Speed->value() * (
                    l + r + u + d - 4 * surfaces[i][j].t1Height
                    ) * tickLength * tickLength;
        }
    }

	//================================================================
	
    float baisHeight = 1.0f;

	int start;
	//position
	start = 0;
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < count; j++) {
            data[start + (i * count + j) * 3 + 0] = surfaces[i + 1][j + 1].x;
            data[start + (i * count + j) * 3 + 1] = surfaces[i + 1][j + 1].height + baisHeight;
            data[start + (i * count + j) * 3 + 2] = surfaces[i + 1][j + 1].y;
        }
    }
	//normal
	start = count * count * 3;
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < count; j++) {
			Pnt3f normal = Pnt3f(0, 0, 0);
			Pnt3f p0 = Pnt3f(surfaces[i + 1][j + 1].x, surfaces[i + 1][j + 1].height, surfaces[i + 1][j + 1].y);
			Pnt3f p1 = Pnt3f(surfaces[i + 1][j + 2].x, surfaces[i + 1][j + 2].height, surfaces[i + 1][j + 2].y);
			Pnt3f p2 = Pnt3f(surfaces[i + 2][j + 2].x, surfaces[i + 2][j + 2].height, surfaces[i + 2][j + 2].y);
			Pnt3f p3 = Pnt3f(surfaces[i + 2][j + 1].x, surfaces[i + 2][j + 1].height, surfaces[i + 2][j + 1].y);
			Pnt3f p4 = Pnt3f(surfaces[i + 1][j + 0].x, surfaces[i + 1][j + 0].height, surfaces[i + 1][j + 0].y);
			Pnt3f p5 = Pnt3f(surfaces[i + 0][j + 0].x, surfaces[i + 0][j + 0].height, surfaces[i + 0][j + 0].y);
			Pnt3f p6 = Pnt3f(surfaces[i + 0][j + 1].x, surfaces[i + 0][j + 1].height, surfaces[i + 0][j + 1].y);
			
			normal = normal +
				((p0 + p2 * -1) * (p1 + p0 * -1)) +
				((p0 + p3 * -1) * (p2 + p0 * -1)) +
				((p0 + p4 * -1) * (p3 + p0 * -1)) +
				((p0 + p5 * -1) * (p4 + p0 * -1)) +
				((p0 + p6 * -1) * (p5 + p0 * -1)) +
				((p0 + p1 * -1) * (p6 + p0 * -1));

			normal.normalize();

			data[start + (i * count + j) * 3 + 0] = normal.x;
			data[start + (i * count + j) * 3 + 1] = normal.y;
			data[start + (i * count + j) * 3 + 2] = normal.z;
		}
	}

	
	glBindVertexArray(this->vao->vao);

	//position
	glBindBuffer(GL_ARRAY_BUFFER, vao->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, this->count * this->count * (3 + 3 + 2) * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	
	//normal
	glBindBuffer(GL_ARRAY_BUFFER, vao->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, this->count * this->count * (3 + 3 + 2) * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)((count * count * 3) * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//texture cube
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->id);
	glBindVertexArray(0);
	
	glm::vec4 eye{
		-TrainWindow::magic->trainView->arcball.eyeX,
		-TrainWindow::magic->trainView->arcball.eyeY,
		-TrainWindow::magic->trainView->arcball.eyeZ,
		1.0f
	};

	HMatrix Hmatrix; //wtf???
	Quat qAll = TrainWindow::magic->trainView->arcball.now * TrainWindow::magic->trainView->arcball.start;
	qAll.toMatrix(Hmatrix);

	glm::mat4 view_matrix;
	for(int i = 0; i < 4; ++i){for(int j = 0; j < 4; ++j){view_matrix[i][j] = Hmatrix[i][j];}}

	eye = view_matrix * eye;
	eye = -eye;
	
	this->shader->Use();

	glUniform3fv(
		glGetUniformLocation(this->shader->Program, "eye_position"), 1, &eye[0]);
}

void water_t::nextFrame() {
    tickBack();
    fresh();
}

void water_t::touchWater(float height) {
    surfaces[count / 2 + 1][count / 2 + 1].height = height;
}

void water_t::set_skybox(Texture2D* skybox){
	this->skybox = skybox;
}
