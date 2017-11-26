#pragma once

#include <string>
#include <assert.h>
#include "GLUtilities/gl_utils.h"

using namespace std;
class ShaderProgram {
public: 
  GLuint program;

  /* attributes */
  GLuint position_id;
  GLuint offset_id;
  GLuint scale_id;
	GLuint color_id;
	GLuint uv_id;
	GLuint normal_id;

  /* uniforms */
  GLuint matrix_id;
	GLuint model_view_id;
	GLuint projection_id;
	GLuint focal_length_id;
	GLuint ray_origin_id;
	GLuint samples_id;
	GLuint perturbation_id;
  GLuint pointSize_id;
  GLuint color_uniform_id;
	GLuint texture0_id;
	GLuint texture1_id;
	GLuint offset_uniform_id;
	GLuint width_uniform_id;
	GLuint height_uniform_id;
	GLuint window_size_id;

  ShaderProgram(const string vertShader, const string fragShader);
  void getAttributes();
  void getUniforms();
  void use();
};