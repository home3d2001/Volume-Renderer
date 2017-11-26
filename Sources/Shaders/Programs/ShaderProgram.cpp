#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram(const string vertShader, const string fragShader) {
	program = GLUtilities::create_program_from_files(vertShader.c_str(), fragShader.c_str());
	getAttributes();
	getUniforms();
}

void ShaderProgram::getAttributes() {
	position_id = glGetAttribLocation(program, "position");
	offset_id = glGetAttribLocation(program, "offset");
	scale_id = glGetAttribLocation(program, "scale");
	color_id = glGetAttribLocation(program, "color");
	uv_id = glGetAttribLocation(program, "uv");
	normal_id = glGetAttribLocation(program, "normal");
}

void ShaderProgram::getUniforms() {
	matrix_id = glGetUniformLocation(program, "matrix");
	model_view_id = glGetUniformLocation(program, "model_view");
	projection_id = glGetUniformLocation(program, "projection");
	focal_length_id = glGetUniformLocation(program, "focal_length");
	ray_origin_id = glGetUniformLocation(program, "ray_origin");
	pointSize_id = glGetUniformLocation(program, "point_size");
	color_uniform_id = glGetUniformLocation(program, "color");
	texture0_id = glGetUniformLocation(program, "texture0");
	texture1_id = glGetUniformLocation(program, "texture1");
	offset_uniform_id = glGetUniformLocation(program, "u_offset");
	width_uniform_id = glGetUniformLocation(program, "width");
	height_uniform_id = glGetUniformLocation(program, "height");
	window_size_id = glGetUniformLocation(program, "window_size");
	samples_id = glGetUniformLocation(program, "samples");
	perturbation_id = glGetUniformLocation(program, "perturbation");
}

void ShaderProgram::use() {
	glUseProgram(program);
}
