#pragma once

#include "Shaders/Shaders.hpp"
#include "Entities/Entity.h"
#include "Vector/vec.h"
#include "Entities/GUI/Button.hpp"
using namespace GLUtilities;
using namespace std;

namespace Entities {
	class CircleWidget : public Entity {
	public:
		int id;

		void render(glm::mat4, glm::mat4);
		void update();
		void raycast(glm::vec4 p, glm::vec4 d);
		void setBoxPosition(float x, float y);


		CircleWidget();
		void setCenterColor(glm::vec4 newColor);
		void setSideColor(glm::vec4 newColor);
		void setEdgeColor(glm::vec4 newColor);

		vec4 getCenterColor();
		vec4 getSideColor();
		vec4 getEdgeColor();

		bool widgetHit = false;
		bool boxSelected = false;
		bool translatingWidget = false;

		int selectedBox = 0;

		bool renderEdges = true;
		bool renderBoxes = true;
		bool renderTriangles = true;
	protected:
		vector<glm::vec4> trianglePoints;
		vector<glm::vec4> trianglePointColors;
		vector<glm::vec4> edgePoints;
		vector<glm::vec4> edgePointColors;

		vector<std::shared_ptr<Button>> buttons;

		static int count;
		void updateVBO();
		void refreshVBO();
		void updateVAO();

		glm::vec4 centerColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
		glm::vec4 sideColor = glm::vec4(1.0, 0.0, 0.0, 0.0);
		glm::vec4 edgeColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
	private:
		float radius = .2;
		int sides = 10;
		bool refreshRequired = false;
		GLuint edgesVAO, trianglesVAO;
		cl_GLuint trianglePointsVBO;
		cl_GLuint trianglePointColorsVBO;
		cl_GLuint edgePointsVBO;
		cl_GLuint edgePointColorsVBO;
		size_t trianglePointsVBOSize = 0;
		size_t edgePointsVBOSize = 0;
		size_t trianglePointColorsVBOSize = 0;
		size_t edgePointColorsVBOSize = 0;
	};
}