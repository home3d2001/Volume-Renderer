#include "Canvas.hpp"

namespace Entities {
	Canvas::Canvas() {
		using namespace std;
		
		/* Add background panel */
		background = make_shared<Box>();
		background->color = color;
		background->transform.SetScale(1, 1, .0001);
	}

	void Canvas::raycast(glm::vec4 p, glm::vec4 d) {
		//p.x += width / 2.0;
		//p.y += height / 2.0;

		Entity::raycast(p, d);
	}

	void Canvas::render(glm::mat4 parent_matrix, glm::mat4 projection) {
		if (showBackground) {
			background->render(parent_matrix * transform.LocalToParentMatrix(), projection);
		}
		Entity::render(parent_matrix, projection);
	}

	void Canvas::setColor(glm::vec4 color) {
		background->color = color;
	}
}