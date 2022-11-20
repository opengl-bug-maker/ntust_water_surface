#include "gpu_object.h"


void gpu_obj_t::init() {
	
}

void gpu_obj_t::update(float* input_data, size_t index) {
	//input_data -> whole data -> choose perticular segment to update
	int start = 0;
	for(int i = 0; i < index - 1; i++)
		start += data_block_size[i];
	start *= this->vao->element_amount;
	for(int i = 0; i < data_block_size[index] * this->vao->element_amount; i++){
		this->data[start + i] = input_data[i];
	}
}

gpu_obj_t::gpu_obj_t() {

}

gpu_obj_t::gpu_obj_t(GLfloat* input_data) : data(input_data) {

}

gpu_obj_t::gpu_obj_t(glm::mat4 model_matrix) : model_matrix(model_matrix) {
}

gpu_obj_t::gpu_obj_t(GLfloat* input_data, glm::mat4 model_matrix) : data(input_data), model_matrix(model_matrix) {
}

void gpu_obj_t::draw(glm::mat4 modelMatrix) {
	//todo multi parent modelMatrix
	/*for(auto child : children){
		child->draw(model_matrix);
	}*/
	//todo true draw
	this->shader->Use();
	//cerr << type << endl;
	glUniformMatrix4fv( //warning : model matrix name
		glGetUniformLocation(this->shader->Program, "u_model"), 1, GL_FALSE, &modelMatrix[0][0]); 

	glBindVertexArray(vao->vao); //wtf
	//glDrawElements(type, vao->element_amount * (type == GL_QUADS ? 4 : 3), GL_UNSIGNED_INT, 0);
	glDrawElements(type, vao->element_amount * 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}
