#pragma once

#include "Entities/Entity.h"
#include "Entities/Box/Box.hpp"

namespace Entities {
	class Button : public Entity {
	public:
		void render(glm::mat4, glm::mat4);
		Button();
		glm::vec4 color = glm::vec4(.5, .5, .5, 1);
		glm::vec4 hoverColor = glm::vec4(1, 1, 1, 1);
		glm::vec4 clickColor = glm::vec4(0, 0, 0, 1);
		std::shared_ptr<Box> box;
		bool activatable = false;
		bool clicked = false;
		bool held = false;
		bool active = false;
		void raycast(glm::vec4 p, glm::vec4 d);
	};
};