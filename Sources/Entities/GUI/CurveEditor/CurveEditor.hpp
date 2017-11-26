/* CurveEditor.
	NateVM. 2017

	A curve editor allows users to edit the red, blue, green, and alpha intensities of a texture.
		Users select a color, and then draw on the curve editor to edit color intensities.
		This editor is specifically for 2D transfer functions.
*/

#include "Entities/GUI/Canvas.hpp"
#include "Entities/GUI/Button.hpp"
#include "Entities/ImagePlane/ImagePlane.hpp"

namespace Entities {
	using namespace glm;
	using namespace std;

	/* Four channels, which can be selected for editing */
	enum ColorChannel {RED, GREEN, BLUE, ALPHA};
	class CurveEditor : public Entity {
	public:
		CurveEditor(shared_ptr<Texture> texture, shared_ptr<Texture> histogram, float aspectRatio);

		void render(mat4 parent_matrix, mat4 projection);
		void update();
		void raycast(vec4 p, vec4 d);

		void setBoxPosition(int index, float yPos);
		
		void setChannel(ColorChannel channel);

		/* Canvas is shown under the texture */
		shared_ptr<Canvas> canvas;

		/* Texture being modified */
		shared_ptr<Texture> texture;

	private:
		void addButtons();
		void initializeTransferFunctionGeometry();
		void initializeFrameBuffer();
		void updateVAO();
		void updateVBO();
		
		int editChannel = 3;
		void setColor(int index, float intensity);

		vector<shared_ptr<Box>> redBoxes, blueBoxes, greenBoxes, alphaBoxes;

		float aspectRatio;
		int totalBoxes = 256;
		float boxSize = .01;

		int lastClickedBox = 0;

		bool refresh = false;
		GLuint VAO = 0;
		cl_GLuint pointsVBO = 0;
		cl_GLuint colorsVBO = 0;
		size_t pointsVBOSize = 0;
		size_t colorsVBOSize = 0;
		
		vector<vec4> points;
		vector<vec4> colors;

		GLuint frameBuffer;
		GLuint depthBuffer;
		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };

	};
}