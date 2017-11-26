#pragma once

#include "Entities/Entity.h"
#include "Entities/Box/Box.hpp"

namespace Entities {
	class Canvas : public Entity {
	public:
		double width, height;
		
		void render(glm::mat4, glm::mat4);
		void raycast(glm::vec4 p, glm::vec4 d);
		Canvas();
		bool showBackground = true;
		
		void setColor(glm::vec4 color);
	private:
		std::shared_ptr<Box> background;
		glm::vec4 color = glm::vec4(1.0, 1.0, 0.0, 1.0);

	};
};