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

#include "Transform.h"

namespace Entities {
	class Entity {
	public:
		Transform transform = Transform();

		///* Scene graph fields */
		//glm::mat4 M;
		//glm::mat4 IM;

		//glm::vec3 Scale = glm::vec3(1,1,1);
		//glm::vec3 Position = glm::vec3(0,0,0);
		//glm::vec3 Rotation = glm::vec3(0,0,0);

		std::unordered_map<std::string, std::shared_ptr<Entity>> objects;
		bool renderable = true;
		bool toggled = true;
		virtual void render(glm::mat4 parent_matrix = glm::mat4(), glm::mat4 projection = glm::mat4()) {
			glm::mat4 final_matrix = parent_matrix * transform.LocalToParentMatrix();
			for (auto i : objects) {
				if (i.second.get()->toggled) {
					print_gl_error_w_id("before " + i.first + " render");
					i.second->render(final_matrix, projection);
					print_gl_error_w_id("after " + i.first + " render");
				}
			}
		};
		virtual void update() {
			for (auto i : objects) {
				if (i.second.get()->toggled) {
					i.second->update();
				}
			}
		};
	
		/* Each entity receives a recursive point and direction, such that p and d */
		virtual void raycast(glm::vec4 point, glm::vec4 direction) {
			for (auto i : objects) {
				if (i.second.get()->toggled) {
					/* Transform the ray */
					glm::vec4 newPoint = i.second->transform.ParentToLocalMatrix() * point;
					glm::vec4 newDirection = (i.second->transform.ParentToLocalMatrix() * (point + direction)) - newPoint;
					i.second->raycast(newPoint, newDirection);
				}
			}
		}
		
		void add(const string key, shared_ptr<Entity> object) {
			objects[key] = object;
		}
		void remove(const string key) {
			objects.erase(key);
		}
		void toggle(const std::string key) {
			objects.at(key).get()->toggled = !objects.at(key).get()->toggled;
		}
		/*void move(const string key, glm::mat4 matrix) {
			objects[key]->M = matrix;
		}*/

		/*void rotate(glm::quat rotation) {
			glm::mat4 RotationMatrix = glm::toMat4(rotation);
			M = M * RotationMatrix;
			IM = glm::inverse(M);
		}
		void scale(glm::vec3 scale_vector) {
			size *= scale_vector;

			M = glm::scale(M, scale_vector);
			IM = glm::inverse(M);
		}
		void scale(float x, float y, float z) {
			scale({ x,y,z });
		}
		void translate(glm::vec3 translate_vector) {
			position += translate_vector;
			M = glm::translate(M, translate_vector);
			IM = glm::inverse(M);
		}
		void translate(float x, float y, float z) {
			translate({x, y, z});
		}

		void setTranslation(glm::vec3 translate_vector) {
			position += translate_vector;
			M = glm::translate(M, translate_vector);
			IM = glm::inverse(M);
		}*/
	};
}
