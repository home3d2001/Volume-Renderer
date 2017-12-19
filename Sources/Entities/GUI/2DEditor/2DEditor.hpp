/* CurveEditor.
NateVM. 2017


*/

#include "Entities/GUI/Canvas.hpp"
#include "Entities/GUI/Button.hpp"
#include "Entities/ImagePlane/ImagePlane.hpp"
#include "CircleWidget.hpp"


namespace Entities {
	using namespace glm;
	using namespace std;

	/* Four channels, which can be selected for editing */
	class TwoDimensionEditor : public Entity {
	public:
		TwoDimensionEditor(std::shared_ptr<Texture> texture, shared_ptr<Texture> histogramTexture, int2 histTopLeft, int2 histBotRight, float aspectRatio = 1.0);

		void render(mat4 parent_matrix, mat4 projection);
		void update();
		void raycast(vec4 p, vec4 d);

		vec4 getSelectedWidgetCenterColor();
		void setSelectedWidgetCenterColor(vec4 color);

		vec4 getSelectedWidgetSideColor();
		void setSelectedWidgetSideColor(vec4 color);
		void addWidget();
		void deleteSelectedWidget();

		void handleKeys();

		/* Canvas is shown under the texture */
		shared_ptr<Canvas> canvas;

		/* Texture being modified */
		shared_ptr<Texture> texture;

		bool widgetSelected = false;
		bool boxSelected = false;
		vec3 offset;

		bool addingWidget = false;
		int deletingWidget = -1;
	private:
		std::vector<std::shared_ptr<CircleWidget>> widgets;

		float aspectRatio;

		void updateVAO();
		void updateVBO();
		bool refresh = false;

		void initializeFrameBuffer();
		int selectedWidget = 0;

		/*
		void setColor(int index, float intensity);

		int totalBoxes = 256;
		float boxSize = .01;

		int lastClickedBox = 0;

		GLuint VAO = 0;
		cl_GLuint pointsVBO = 0;
		cl_GLuint colorsVBO = 0;
		size_t pointsVBOSize = 0;
		size_t colorsVBOSize = 0;

		vector<vec4> points;
		vector<vec4> colors;

*/
		GLuint frameBuffer;
		GLuint depthBuffer;
		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	};
}