#pragma once
#include <opencv2\opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
using namespace std;
using namespace cv;
class skybox_t{
public:
	enum Type {
		TEXTURE_DEFAULT = 0,
		TEXTURE_DIFFUSE, TEXTURE_SPECULAR,
		TEXTURE_NORMAL, TEXTURE_DISPLACEMENT,
		TEXTURE_HEIGHT,
	};

	Type type;

	skybox_t(vector<string> paths, Type texture_type = skybox_t::TEXTURE_DEFAULT) :
		type(texture_type)
	{
		//cv::imread(path, cv::IMREAD_COLOR).convertTo(img, CV_32FC3, 1 / 255.0f);	//unsigned char to float
		Mat imgs[6];
		for (int i = 0; i < 6; ++i) {
			imgs[i] = cv::imread(paths[i], cv::IMREAD_COLOR);
		}

		this->size.x = imgs[0].cols;
		this->size.y = imgs[0].rows;

	
		glGenTextures(1, &this->id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->id);

		int width, height, nrComponents;
		width = imgs[0].cols;
		height = imgs[0].rows;
		for (unsigned int i = 0; i < 6; i++){
			if (imgs[i].data){
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, imgs[i].data);
				//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, img.cols, img.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
				imgs[i].release();
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	void bind(GLenum bind_unit)
	{
		glActiveTexture(GL_TEXTURE0 + bind_unit);
		glBindTexture(GL_TEXTURE_2D, this->id);
	}
	static void unbind(GLenum bind_unit)
	{
		glActiveTexture(GL_TEXTURE0 + bind_unit);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glm::ivec2 size;
private:
	GLuint id;

};



class Texture2D
{
public:
	enum Type {
		TEXTURE_DEFAULT = 0,
		TEXTURE_DIFFUSE, TEXTURE_SPECULAR,
		TEXTURE_NORMAL, TEXTURE_DISPLACEMENT,
		TEXTURE_HEIGHT,
	};

	Type type;

	Texture2D(const char* path, Type texture_type = Texture2D::TEXTURE_DEFAULT):
		type(texture_type)
	{
		cv::Mat img;
		//cv::imread(path, cv::IMREAD_COLOR).convertTo(img, CV_32FC3, 1 / 255.0f);	//unsigned char to float
		img = cv::imread(path, cv::IMREAD_COLOR);

		this->size.x = img.cols;
		this->size.y = img.rows;

		//cv::cvtColor(img, img, CV_BGR2RGB);

		glGenTextures(1, &this->id);

		glBindTexture(GL_TEXTURE_2D, this->id);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		if(img.type() == CV_8UC3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, img.cols, img.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
		else if (img.type() == CV_8UC4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img.cols, img.rows, 0, GL_BGRA, GL_UNSIGNED_BYTE, img.data);
		glBindTexture(GL_TEXTURE_2D, 0);

		img.release();
	}
	void bind(GLenum bind_unit)
	{
		glActiveTexture(GL_TEXTURE0 + bind_unit);
		glBindTexture(GL_TEXTURE_2D, this->id);
	}
	static void unbind(GLenum bind_unit)
	{
		glActiveTexture(GL_TEXTURE0 + bind_unit);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glm::ivec2 size;
private:
	GLuint id;

};