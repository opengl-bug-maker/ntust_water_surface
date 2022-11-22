#include "gpu_object.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
class wall_t : public gpu_obj_t {
	void init() override;

public:
	void bind() override;
	wall_t();
	wall_t(GLfloat* input_data);
};