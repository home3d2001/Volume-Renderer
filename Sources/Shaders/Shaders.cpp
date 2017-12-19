#include "Shaders.hpp"
namespace Shaders {
	ShaderProgram* slicedVolProgram;
	ShaderProgram* raycastVolProgram;
	ShaderProgram* planeProgram;
	ShaderProgram* boxProgram;
  ShaderProgram* pointProgram;
	ShaderProgram* lineProgram;
	ShaderProgram* uniformColorProgram;
	ShaderProgram* NonShaded2VBO;
  //ShaderProgram* sketchBoxProgram;
  void create() {
		slicedVolProgram = new ShaderProgram("./Sources/OpenGL/Shaders/slicedVol.vert", "./Sources/OpenGL/Shaders/slicedVol.frag");
		raycastVolProgram = new ShaderProgram("./Sources/OpenGL/Shaders/raycastVol.vert", "./Sources/OpenGL/Shaders/raycastVol.frag");
		planeProgram = new ShaderProgram("./Sources/OpenGL/Shaders/plane.vert", "./Sources/OpenGL/Shaders/plane.frag");
		boxProgram = new ShaderProgram("./Sources/OpenGL/Shaders/boxes.vert", "./Sources/OpenGL/Shaders/boxes.frag");
		lineProgram = new ShaderProgram("./Sources/OpenGL/Shaders/lines.vert", "./Sources/OpenGL/Shaders/lines.frag");
		uniformColorProgram = new ShaderProgram("./Sources/OpenGL/Shaders/uniformColor.vert", "./Sources/OpenGL/Shaders/uniformColor.frag");
		pointProgram = new ShaderProgram("./Sources/OpenGL/Shaders/points.vert", "./Sources/OpenGL/Shaders/points.frag");
		NonShaded2VBO = new ShaderProgram("./Sources/OpenGL/Shaders/NonShaded2VBO.vert", "./Sources/OpenGL/Shaders/NonShaded2VBO.frag");
    //sketchBoxProgram = new ShaderProgram("./Sources/OpenGL/Shaders/sketchBox.vert", "./Sources/OpenGL/Shaders/sketchBox.frag");
  }
}
