#pragma once
#include "gpu_object.h"
#include "Utilities/Pnt3f.H"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
class TrainWindow;

class surface {
public:
    float t2Height = 0;
    float t1Height = 0;
    float height = 0;
	float x = 0;
	float y = 0;
};

class water_t : public gpu_obj_t {
	void init() override;

    int count = 30;
    surface** surfaces;

    void tickBack();
    void fresh();
    void glFresh();

public:
	void bind() override;
    //skybox_t* skybox;
    Texture2D* skybox;
	water_t();
	water_t(GLfloat* input_data);
   
    TrainWindow* tw;

    void sineWave(int t);
    void nextFrame();
    void touchWater(float height);
    void mouseTouchWater(int x, int y, float force);
    //void set_skybox(skybox_t*);
    void set_skybox(Texture2D*);
};