#include "gpu_object.h"

class wall_t : public gpu_obj_t {
	void init() override;

public:
	void bind() override;
	wall_t();
	wall_t(GLfloat* input_data);
};