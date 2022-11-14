#include "water.h"
#include "TrainWindow.h"
#include "TrainView.H"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glu.h>
#include <iostream>
#include "Utilities/3DUtils.H"
using namespace std;
water::water(TrainWindow* tw) : tw(tw) {
    count = 60;
    init();
}
void water::init() {
	float step = 20.0f / count;
    surfaces = new surface * [count + 2];
    for (int i = 0; i < count + 2; ++i) {
        surfaces[i] = new surface[count + 2];
		for (int j = 0; j < count + 2; ++j) {
			surfaces[i][j].x = -10.0f + step * (i - 1);
			surfaces[i][j].y = -10.0f + step * (j - 1);
		}
    }
    Heights = new GLfloat[count * count * 3];
	NormalVectors = new GLfloat[count * count * 3];

	Textures = new GLfloat[count * count * 2];
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < count; j++) {
			Textures[(i * count + j) * 2 + 0] = (surfaces[i + 1][j + 1].x + 10.0f) / 20.0f;
			Textures[(i * count + j) * 2 + 1] = -(surfaces[i + 1][j + 1].y + 10.0f) / 20.0f;
			cout << Textures[(i * count + j) * 2 + 0] << Textures[(i * count + j) * 2 + 1] << endl;
		}
	}


    Elements = new GLuint[(count - 1) * (count - 1) * 2 * 3];
	int index = 0;
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1; j++) {
            Elements[index++] = i * count + j;
            Elements[index++] = (i + 1) * count + (j + 1);
            Elements[index++] = (i + 1) * count + j;

            Elements[index++] = i * count + j;
            Elements[index++] = i * count + (j + 1);
            Elements[index++] = (i + 1) * count + (j + 1);
        }
    }
	//if (!this->shader) {
	//	this->shader = new Shader(
	//		PROJECT_DIR "/src/shaders/water.vert",
	//		nullptr, nullptr, nullptr,
	//		PROJECT_DIR "/src/shaders/water.frag");
	//}
}

void water::GL_Init(){
    if (!this->shader) {
		this->shader = new Shader(
            PROJECT_DIR "/src/shaders/water.vert",
            nullptr, nullptr, nullptr,
            PROJECT_DIR "/src/shaders/water.frag");
    }
	if (!this->data) {
		this->data = new VAO;
		this->data->element_amount = (count - 1) * (count - 1) * 2 * 3 * sizeof(GLuint) / sizeof(GLuint);

		glGenVertexArrays(1, &this->data->vao);
		glGenBuffers(3, this->data->vbo);
		glGenBuffers(1, &this->data->ebo);

		glBindVertexArray(this->data->vao);

		//position
		glBindBuffer(GL_ARRAY_BUFFER, this->data->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, count * count * 3 * sizeof(GLfloat), Heights, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

		//normal
		glBindBuffer(GL_ARRAY_BUFFER, this->data->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, count * count * 3 * sizeof(GLfloat), NormalVectors, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

		//texture
		glBindBuffer(GL_ARRAY_BUFFER, this->data->vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, count * count * 2 * sizeof(GLfloat), Textures, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->data->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (count - 1) * (count - 1) * 2 * 3 * sizeof(GLuint), Elements, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}
    //texture
    //glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[2]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(wall_texture_coordinate), wall_texture_coordinate, GL_STATIC_DRAW);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0* sizeof(GLfloat), (GLvoid*)0);
    //glEnableVertexAttribArray(2);

    //color
    //glBindBuffer(GL_ARRAY_BUFFER, this->data->vbo[1]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(color), wall_color, GL_STATIC_DRAW);
    //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)0);
    //glEnableVertexAttribArray(3);
    
	if (!this->commom_matrices){
		this->commom_matrices = new UBO();
		this->commom_matrices->size = 2 * sizeof(glm::mat4);
		glGenBuffers(1, &this->commom_matrices->ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
		glBufferData(GL_UNIFORM_BUFFER, this->commom_matrices->size, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

	if (!this->texture)
		this->texture = new Texture2D(PROJECT_DIR "/Images/tiles.jpg");
}

void water::tickBack() {
    for (int i = 0; i < count + 2; i++) {
        for (int j = 1; j < count + 2; j++) {
            surfaces[i][j].t2Height = surfaces[i][j].t1Height;
            surfaces[i][j].t1Height = surfaces[i][j].height;
        }
    }
}

void water::fresh() {
    for (int i = 1; i <= count; ++i) {
        for (int j = 1; j <= count; ++j) {
            float l = surfaces[i - 1][j].t1Height;
            float r = surfaces[i + 1][j].t1Height;
            float u = surfaces[i][j + 1].t1Height;
            float d = surfaces[i][j - 1].t1Height;

            float tickLength = 1;

            // surfaces[i][j].height = (1 + slowdown) * surfaces[i][j].t1Height -
            //     slowdown * surfaces[i][j].t2Height +

            //     0.5f * speed * speed * (
            //         l + r + u + d - 4 * surfaces[i][j].t1Height
            //         ) * tickLength * tickLength;
            surfaces[i][j].height = (1 + tw->SlowDown->value()) * surfaces[i][j].t1Height -
                tw->SlowDown->value() * surfaces[i][j].t2Height +

                0.5f * tw->Speed->value() * tw->Speed->value() * (
                    l + r + u + d - 4 * surfaces[i][j].t1Height
                    ) * tickLength * tickLength;
        }
    }
}

void water::nextFrame() {
    tickBack();
    fresh();
}
void water::touchWater(float height) {
    surfaces[count / 2 + 1][count / 2 + 1].height = height;
}


void water::GL_Draw() {
    float step = 20.0f / count;
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < count; j++) {
            Heights[(i * count + j) * 3 + 0] = surfaces[i + 1][j + 1].x;
            Heights[(i * count + j) * 3 + 1] = surfaces[i + 1][j + 1].height;
            Heights[(i * count + j) * 3 + 2] = surfaces[i + 1][j + 1].y;
        }
    }
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

			NormalVectors[(i * count + j) * 3 + 0] = normal.x;
			NormalVectors[(i * count + j) * 3 + 1] = normal.y;
			NormalVectors[(i * count + j) * 3 + 2] = normal.z;
		}
	}
	//for (int i = 0; i < count; ++i) {
	//	for (int j = 0; j < count; ++j) {
	//		cout << NormalVectors[(i * count + j) * 3 + 0] << " ";
	//		cout << NormalVectors[(i * count + j) * 3 + 1] << " ";
	//		cout << NormalVectors[(i * count + j) * 3 + 2] << endl;
	//	}
	//}

	glm::mat4 view_matrix;
	glGetFloatv(GL_MODELVIEW_MATRIX, &view_matrix[0][0]);

	glm::mat4 projection_matrix;
	glGetFloatv(GL_PROJECTION_MATRIX, &projection_matrix[0][0]);

	glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &projection_matrix[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view_matrix[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, /*binding point*/0, this->commom_matrices->ubo, 0, this->commom_matrices->size);

	this->shader->Use();

	//position
	//todo : fuck up
	glBindBuffer(GL_ARRAY_BUFFER, this->data->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, count * count * 3 * sizeof(GLfloat), Heights, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->data->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, count * count * 3 * sizeof(GLfloat), NormalVectors, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->data->vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, count * count * 2 * sizeof(GLfloat), Textures, GL_DYNAMIC_DRAW);
	

	glm::mat4 model_matrix = glm::mat4();
	model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 5.0f, 0.0f));
	model_matrix = glm::scale(model_matrix, glm::vec3(10.0f, 10.0f, 10.0f));

	glUniformMatrix4fv(
		glGetUniformLocation(this->shader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);
	glUniform3fv(
		glGetUniformLocation(this->shader->Program, "u_color"),
		1,
		&glm::vec3(0.0f, 1.0f, 0.0f)[0]);
	glUniform3fv(
		glGetUniformLocation(this->shader->Program, "light_color"), 1, &glm::vec3(1.0f, 0.7f, 1.0f)[0]);
	glUniform3fv(
		glGetUniformLocation(this->shader->Program, "lightPos"), 1, &glm::vec3(0.0f, 100.0f, 100.0f)[0]);
	glUniform3fv(
		glGetUniformLocation(this->shader->Program, "eye_position"), 1, &glm::vec3(0.0f, 100.0f, 0.0f)[0]);
	this->texture->bind(0);
	glUniform1i(glGetUniformLocation(this->shader->Program, "u_texture"), 0);



    glBindVertexArray(this->data->vao);
    glDrawElements(GL_TRIANGLES, this->data->element_amount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);
}