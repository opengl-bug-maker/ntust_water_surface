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

public:
	void bind() override;
	water_t();
	water_t(GLfloat* input_data);
    
    TrainWindow* tw;

    void nextFrame();
    void touchWater(float height);
};