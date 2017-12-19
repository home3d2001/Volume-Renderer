#include "Button.hpp"

namespace Entities {
	Button::Button() {
		using namespace std;
				
		/* Add background panel */
		box = make_shared<Box>();
		box->color = color;
		box->transform.SetScale(1.0, 1.0, 1.0);
	}
	
	void Button::raycast(glm::vec4 p, glm::vec4 d) {
		if (!activatable) { active = false; }
		if (active == true) box->color = clickColor;

		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if (state != GLFW_PRESS) {
			clicked = false;
			held = false;
		}

		if (p.x > -1.0 && p.x < 1.0 &&
			p.y > -1.0 && p.y < 1.0
			) {
			box->color = hoverColor;

			if (state == GLFW_PRESS) {
				active = true;
				box->color = clickColor;
				if (clicked == false && held == false) {
					clicked = true;
				}
				else {
					held = true;
					clicked = false;
				}
			}
		}
		else {
			box->color = color;
		}

		
	}

	void Button::render(glm::mat4 parent_matrix, glm::mat4 projection) {
		box->render(parent_matrix * transform.localToParentMatrix.load(), projection);
		Entity::render(parent_matrix, projection);
	}
}