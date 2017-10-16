#include "OrthoCamera.h"

namespace GLUtilities {
	float OrthoCamera::left = -1.;
	float OrthoCamera::right = 1.;
	float OrthoCamera::bottom = -1.;
	float OrthoCamera::top = 1.;

	glm::mat4 OrthoCamera::V = glm::mat4(1.0);
	glm::mat4 OrthoCamera::IV = glm::mat4(1.0);

	glm::mat4 OrthoCamera::zoom(glm::vec2 origin, float percent) {
		using namespace glm;
		
		/* Center at origin. Then scale. Then undo translation. */
		V = glm::translate(V, vec3(origin, 0.0));
		V = glm::scale(V, vec3(percent, percent, 1.0));
		V = glm::translate(V, vec3(-origin, 0.0));
		IV = glm::inverse(V);
		return V;
	}
	glm::mat4 OrthoCamera::pan(glm::vec2 displacement) {
		using namespace glm;
		V = translate(V, vec3(displacement, 0.0));
		IV = glm::inverse(V);
		return V;
	}
	glm::mat4 OrthoCamera::reset() {
		V = glm::ortho(left, right, bottom, top, -1.f, 1.f);
		IV = glm::inverse(V);
		return V;
	}
}