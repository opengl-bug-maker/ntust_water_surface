#pragma once
#include "gpu_object.h"

class TrainWindow;

class surface {
public:
    float t2Height = 0;
    float t1Height = 0;
    float height = 0;
	float x = 0;
	float y = 0;
};

class water : public gpu_object {
    int count = 0;
    float slowdown = 0.7;
    float speed = 0.8;
    float baisHeight = 12.0f;
    

    //float **Height, **t1Height, **t2Height;

    GLfloat* Heights;
	GLfloat* NormalVectors;
	GLfloat* Textures;
    GLuint* Elements;

    surface** surfaces;

    void tickBack();
    void fresh();

protected:
    virtual void init() override;
    
    virtual void GL_Draw() override;

public:
    virtual void GL_Init() override;

    TrainWindow* tw;
    
    water(TrainWindow* tw);
    void nextFrame();
    void touchWater(float height);
};