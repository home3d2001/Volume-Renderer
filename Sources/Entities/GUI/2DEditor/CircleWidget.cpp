#include "CircleWidget.hpp"
#include "Vector/vec.h"
#include "Entities/Cameras/OrbitCamera.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Entities {

	int CircleWidget::count = 0;


	CircleWidget::CircleWidget()
	{
		id = count;
		count++;

		updateVBO();
		updateVAO();
	}
	
	void CircleWidget::setCenterColor(glm::vec4 newColor) {
		centerColor = newColor;
		for (int i = 0; i < trianglePointColors.size(); ++i) {
			if ((i % 3) == 1) trianglePointColors.at(i) = centerColor;
		}

		refreshRequired = true;
	}

	void CircleWidget::setSideColor(glm::vec4 newColor) {
		sideColor = newColor;
		for (int i = 0; i < trianglePointColors.size(); ++i) {
			if ((i % 3) != 1) trianglePointColors.at(i) = sideColor;
		}
		refreshRequired = true;
	}

	void CircleWidget::setEdgeColor(glm::vec4 newColor) {
		edgeColor = newColor;
		for (int i = 0; i < edgePointColors.size(); ++i) {
			edgePointColors.at(i) = edgeColor;
		}
		refreshRequired = true;

	}

	vec4 CircleWidget::getCenterColor() {
		return centerColor;
	}

	vec4 CircleWidget::getSideColor() {
		return sideColor;
	}
	
	vec4 CircleWidget::getEdgeColor() {
		return edgeColor;
	}

	inline glm::vec4 polToCart(float theta, float radius)
	{
		glm::vec4 p;
		p.x =  radius*cos(theta);
		p.y = radius*sin(theta);
		p.z = 0.0;
		p.w = 1.0;
		return p;
	}

	void CircleWidget::updateVBO() {
		using namespace glm;
		using namespace std;

		trianglePoints.clear();
		edgePoints.clear();
		trianglePointColors.clear();
		edgePointColors.clear();
		buttons.clear();

		/* Create the geometry for the circle widget's triangles and edges */
		float dtheta = (2.0 * M_PI) / (float)sides;
		for (int i = 0; i < sides; ++i) {
			vec4 centerPoint = vec4(0, 0, 0, 1);
			vec4	P1 = polToCart((i + 1)*dtheta, radius);
			vec4	P2 = polToCart(i*dtheta, radius);
			
			std::shared_ptr<Button> button = make_shared<Button>();
			button->transform.SetPosition(P1.x, P1.y, 0);
			button->transform.SetScale(.03, .03, .01);
			button->color = edgeColor;
			button->clickColor = edgeColor;
			button->hoverColor = edgeColor;
			buttons.push_back(button);
			add("button" + std::to_string(i), button);

			vec4	centerCol = centerColor;
			vec4	P1Col = sideColor;
			vec4	P2Col = sideColor; 

			trianglePoints.push_back(P2);
			trianglePoints.push_back(centerPoint);
			trianglePoints.push_back(P1);
			trianglePointColors.push_back(P1Col);
			trianglePointColors.push_back(centerCol);
			trianglePointColors.push_back(P2Col);
			

			edgePoints.push_back(P1);
			edgePoints.push_back(P2);
			edgePointColors.push_back(edgeColor);
			edgePointColors.push_back(edgeColor);
		}
		
		trianglePointsVBOSize = trianglePoints.size() * sizeof(vec4);
		trianglePointColorsVBOSize = trianglePointColors.size() * sizeof(vec4);
		edgePointsVBOSize = edgePoints.size() * sizeof(vec4);
		edgePointColorsVBOSize = edgePointColors.size() * sizeof(vec4);
		print_gl_error();

		/* Upload the data */
		glDeleteBuffers(1, &trianglePointsVBO);
		glDeleteBuffers(1, &trianglePointColorsVBO);
		glDeleteBuffers(1, &edgePointsVBO);
		glDeleteBuffers(1, &edgePointColorsVBO);
		print_gl_error();

		glGenBuffers(1, &trianglePointsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, trianglePointsVBO);
		glBufferData(GL_ARRAY_BUFFER, trianglePointsVBOSize, trianglePoints.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &trianglePointColorsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, trianglePointColorsVBO);
		glBufferData(GL_ARRAY_BUFFER, trianglePointColorsVBOSize, trianglePointColors.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &edgePointsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, edgePointsVBO);
		glBufferData(GL_ARRAY_BUFFER, edgePointsVBOSize, edgePoints.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &edgePointColorsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, edgePointColorsVBO);
		glBufferData(GL_ARRAY_BUFFER, edgePointColorsVBOSize, edgePointColors.data(), GL_STATIC_DRAW);
		print_gl_error();

	}

	void CircleWidget::updateVAO() {
		print_gl_error();

		glDeleteVertexArrays(1, &edgesVAO);
		glDeleteVertexArrays(1, &trianglesVAO);

		glGenVertexArrays(1, &edgesVAO);
		glGenVertexArrays(1, &trianglesVAO);

		print_gl_error();

		glBindVertexArray(trianglesVAO);
		glBindBuffer(GL_ARRAY_BUFFER, trianglePointsVBO);
		glEnableVertexAttribArray(Shaders::NonShaded2VBO->position_id);
		glVertexAttribPointer(Shaders::NonShaded2VBO->position_id, 4,
			GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, trianglePointColorsVBO);
		glEnableVertexAttribArray(Shaders::NonShaded2VBO->color_id);
		glVertexAttribPointer(Shaders::NonShaded2VBO->color_id, 4,
			GL_FLOAT, GL_FALSE, 0, NULL);

		glBindVertexArray(edgesVAO);
		glBindBuffer(GL_ARRAY_BUFFER, edgePointsVBO);
		glEnableVertexAttribArray(Shaders::NonShaded2VBO->position_id);
		glVertexAttribPointer(Shaders::NonShaded2VBO->position_id, 4,
			GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, edgePointColorsVBO);
		glEnableVertexAttribArray(Shaders::NonShaded2VBO->color_id);
		glVertexAttribPointer(Shaders::NonShaded2VBO->color_id, 4,
			GL_FLOAT, GL_FALSE, 0, NULL);

		print_gl_error();
	}

	void CircleWidget::refreshVBO() {
		using namespace glm;
		using namespace std;
		
		/* Upload the data */
		glDeleteBuffers(1, &trianglePointsVBO);
		glDeleteBuffers(1, &trianglePointColorsVBO);
		glDeleteBuffers(1, &edgePointsVBO);
		glDeleteBuffers(1, &edgePointColorsVBO);
		print_gl_error();

		glGenBuffers(1, &trianglePointsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, trianglePointsVBO);
		glBufferData(GL_ARRAY_BUFFER, trianglePointsVBOSize, trianglePoints.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &trianglePointColorsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, trianglePointColorsVBO);
		glBufferData(GL_ARRAY_BUFFER, trianglePointColorsVBOSize, trianglePointColors.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &edgePointsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, edgePointsVBO);
		glBufferData(GL_ARRAY_BUFFER, edgePointsVBOSize, edgePoints.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &edgePointColorsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, edgePointColorsVBO);
		glBufferData(GL_ARRAY_BUFFER, edgePointColorsVBOSize, edgePointColors.data(), GL_STATIC_DRAW);
		print_gl_error();
	}

	void CircleWidget::render(glm::mat4 parent_matrix, glm::mat4 projection) {
		if (refreshRequired) {
			refreshVBO();
			updateVAO();
		}
		
		glm::mat4 finalMatrix = projection * parent_matrix * transform.localToParentMatrix.load();
		Shaders::NonShaded2VBO->use();
		
		/* Edge points*/
		if (renderEdges) {
			glBindVertexArray(edgesVAO);
			glUniformMatrix4fv(Shaders::NonShaded2VBO->matrix_id, 1, 0, &(finalMatrix[0].x));
			glDrawArrays(GL_LINES, 0, edgePointsVBOSize / sizeof(vec4));
			print_gl_error();
		}

		/* Triangles */
		if (renderTriangles) {
			glBindVertexArray(trianglesVAO);
			glUniformMatrix4fv(Shaders::NonShaded2VBO->matrix_id, 1, 0, &(finalMatrix[0].x));
			glDrawArrays(GL_TRIANGLES, 0, trianglePointsVBOSize / sizeof(vec4));
			print_gl_error();
		}

		if (renderBoxes) {
			Entity::render(parent_matrix, projection);
		}
	}

	void CircleWidget::update() {
		// For testing
		//this->rotate(glm::angleAxis(.1f, glm::vec3(1.0, 0.1, 1.0)));

		Entity::update();
	}

	void CircleWidget::raycast(glm::vec4 p, glm::vec4 d) {
		Entity::raycast(p, d);

		/* For now, approximate the widget with a circular */
		if ((p.x*p.x + p.y*p.y) < radius * radius)
			widgetHit = true;
		else widgetHit = false;
		
		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		/* If we're not clicking, either release the widget or a control point */
		if (state != GLFW_PRESS) {
			boxSelected = false;
			translatingWidget = false;
		}
		/* See if we're either hovering or clicking a control point */
		if (boxSelected == false) {
			for (int i = 0; i < buttons.size(); ++i) {
				if (buttons.at(i)->clicked) {
					boxSelected = true;
					selectedBox = i;
				}
			}
		} 
	
		/* Else  if no boxes were selected, see if we're clicking the widget */
	 if (boxSelected == false && state == GLFW_PRESS && widgetHit) {
			translatingWidget = true;
		}

		//std::cout << "box selected: " << boxSelected << " id " << selectedBox << endl;
	}

	void CircleWidget::setBoxPosition(float x, float y) {
		vec4 p = vec4(x, y, 0.0, 1.0);
		p = this->transform.ParentToLocalMatrix() * p;

		buttons.at(selectedBox)->transform.SetPosition(p.x, p.y, .1);


		edgePoints.at(2 * selectedBox) = vec4(p.x, p.y, 0.0, 1.0);
		edgePoints.at((2 * selectedBox + 3) < edgePoints.size() ? (2 * selectedBox + 3) : (2 * selectedBox + 3) - edgePoints.size()) = vec4(p.x, p.y, 0.0, 1.0);


		//trianglePoints.at(2) = vec4(p.x, p.y, 0.0, 1.0);
		//trianglePoints.at(3) = vec4(p.x, p.y, 0.0, 1.0);

		trianglePoints.at(2 + 3 * selectedBox) = vec4(p.x, p.y, 0.0, 1.0);
		trianglePoints.at((3 + 3 * selectedBox) < trianglePoints.size() ? (3 + 3 * selectedBox): (3 + 3 * selectedBox) - trianglePoints.size()) = vec4(p.x, p.y, 0.0, 1.0);

		//trianglePoints.at(1) = vec4(p.x, p.y, 0.0, 1.0);
		//trianglePoints.at(2) = vec4(p.x, p.y, 0.0, 1.0);
		//trianglePoints.at((3 * selectedBox + 4) < trianglePoints.size() ? (3 * selectedBox + 4) : (3 * selectedBox + 4) - trianglePoints.size()) = vec4(p.x, p.y, 0.0, 1.0);



		refreshRequired = true;
	}

}
