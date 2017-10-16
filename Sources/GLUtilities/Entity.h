/* 
	Entities can be added to the scene.
	The scene renders any renderable entities.
*/

#pragma once
#include "Shaders/Shaders.hpp"
#include "clfw.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <vector>
#include <unordered_map>
#include <memory>

class Entity {
	// VAO/VBO per renderable object type. VBO_cl for OpenCL interop.
protected:
	GLuint VAO;
	cl_GLuint VBO;
	cl::BufferGL VBO_cl;
	size_t VBOSize = 0;
public:
	/* Scene graph fields */
	glm::mat4 M;
	std::unordered_map<std::string, std::shared_ptr<Entity>> objects;
	bool renderable = true;
	bool toggled = true;
	virtual void render(glm::mat4 parent_matrix = glm::mat4()) {
		glm::mat4 final_matrix = parent_matrix * M;
		for (auto i : objects) {
			if (i.second.get()->toggled)
				i.second->render(final_matrix);
		}
	};
	virtual void update() {
		for (auto i : objects) {
			if (i.second.get()->toggled)
				i.second->update();
		}
	};
	void add(const string key, shared_ptr<Entity> object) {
		objects[key] = object;
	}
	void remove(const string key) {
		objects.erase(key);
	}
	void toggle(const std::string key) {
		objects.at(key).get()->toggled = !objects.at(key).get()->toggled;
	}
	void move(const string key, glm::mat4 matrix) {
		objects[key]->M = matrix;
	}
};