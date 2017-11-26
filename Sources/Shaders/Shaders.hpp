#pragma once
#include "./Programs/ShaderProgram.hpp"
namespace Shaders {
	extern ShaderProgram* boxProgram;
	extern ShaderProgram* slicedVolProgram;
	extern ShaderProgram* raycastVolProgram;
	extern ShaderProgram* planeProgram;
  extern ShaderProgram* pointProgram;
  extern ShaderProgram* lineProgram;
	extern ShaderProgram* uniformColorProgram;
  //extern ShaderProgram* sketchBoxProgram;
  void create();
}
