#include "SlicedVolume.hpp"
#include "Vector/vec.h"
#include "GLUtilities/OrbitCamera.h"


int SlicedVolume::count = 0;


SlicedVolume::SlicedVolume(
	string filename, 
	int3 rawDimensions, 
	int bytesPerPixel, 
	shared_ptr<OrbitCamera> camera, 
	shared_ptr<ImagePlane> transferFunction, 
	int samplesToTake) 
{
	this->camera = camera;
	this->transferFunction = transferFunction;
	this->filename = filename;
	count++;
	offset = make_float4(0.0, 0.0, 0.0, 0.0);
	this->rawDimensions = rawDimensions;

	this->samplesToTake = samplesToTake;

	updateVBO();
	updateVAO();
	updateImage(filename, rawDimensions, bytesPerPixel);
}

void SlicedVolume::updateVBO() {
	using namespace glm;
	using namespace std;

	vec3 pos = camera->getPosition();
	vec3 cameraLeft = camera->getLeft();
	vec3 frwd = camera->getForward();

	/* Find intersection with a 1 by 1 by 1 axis aligned cube at the world origin. */
	float tmin, tmax;
	intersectBox({ -.5, -.5, -.5 }, { .5, .5, .5 }, 
		{ pos.x, pos.y, pos.z }, { frwd.x, frwd.y, frwd.z }, tmin, tmax);

	/* Start and end points for the ray */
	vec4 start = vec4(pos + tmin * frwd, 1);
	vec4 end = vec4(pos + tmax * frwd, 1);

	/* Vectors to store different points */
	std::vector<vec4> samplePoints;
	std::vector<vec4> centerPoints;
	std::vector<vec4> facePoints;

	const vec3 directions[3] = {
		{ 1, 0, 0 },
		{ 0, 1, 0 },
		{ 0, 0, 1 },
	};

	float dAlpha = 1.0 / samplesToTake;
	for (int i = 0; i < samplesToTake; ++i) {
		vector<pair<vec4, float>> pointsOnCurrentPlane;
		float alpha = i * dAlpha;
		vec4 sPt = (start * alpha) + (end * (1 - alpha));
		
		/* Add sample point */
		samplePoints.push_back(sPt);
		samplePoints.push_back(vec4( 1.0, 1.0, 1.0, 1.0 ));

		// Ray Plane Intersection
		//    (p0 - l0) . n
		//t = ------------- 
		//        l . n
		// parallel when l.n close to zero
		float t;
		for (int dim = 0; dim < 3; ++dim) {
			vec3 dir = directions[dim];
			float point[3] = { .5, .5, .5 };
			point[dim] = -.5; // project onto dim.

			for (int p = 0; p < 4; ++p) {
				int temp = p;
				if (dim != 0) {
					point[0] = (temp & 1) ? point[0] : -point[0];
					temp++;
				}
				if (dim != 1) {
					point[1] = (temp & 1) ? point[1] : -point[1];
					temp++;
				}
				if (dim != 2) {
					point[2] = (temp & 1) ? point[2] : -point[2];
					temp++;
				}

				float t;
				if (intersectPlane({ frwd.x, frwd.y, frwd.z }, { sPt.x, sPt.y, sPt.z }, 
					{ point[0], point[1], point[2] }, { dir.x, dir.y, dir.z }, 0, 1, t))
				{
					vec3 edgePt = { point[0], point[1], point[2] };
					edgePt += dir * t;

					float angle = pseudoangle2({ sPt.x, sPt.z }, { edgePt.x, edgePt.z });
					pointsOnCurrentPlane.push_back({ { edgePt.x, edgePt.y, edgePt.z, 1.0 }, angle });
				}
			}
		}

		// really ghetto bubble sort. fewer than 6 pts, so this should be okay
		for (int n = 0; n < pointsOnCurrentPlane.size(); ++n) {
			for (int n2 = 0; n2 < pointsOnCurrentPlane.size(); n2++) {
				if (pointsOnCurrentPlane[n].second < pointsOnCurrentPlane[n2].second) {
					pair<vec4, float> temp = pointsOnCurrentPlane[n];
					pointsOnCurrentPlane[n] = pointsOnCurrentPlane[n2];
					pointsOnCurrentPlane[n2] = temp;
				}
			}
		}

		/* Compute centroid */
		vec4 centroid;
		for (int n = 0; n < pointsOnCurrentPlane.size(); ++n) {
			centroid += pointsOnCurrentPlane.at(n).first;
		}
		centroid /= pointsOnCurrentPlane.size();

		/*for (int j = 0; j < pointsOnPlane.size(); ++j) {
			edgePoints.push_back(pointsOnPlane[j].first);
			edgePoints.push_back({ 0.0, 1.0, 0.0, 1.0 });
		}*/
		/* Add triangles to draw. Note, equal number of trangles as there are border points */
		for (int j = 0; j < pointsOnCurrentPlane.size(); ++j) {
			vec4 p1 = pointsOnCurrentPlane.at(j).first;
			vec4 p2 = pointsOnCurrentPlane.at((j == pointsOnCurrentPlane.size() - 1) ? 0 : j + 1).first;

			facePoints.push_back({ p1.x, p1.y, p1.z, 1.0 });
			facePoints.push_back(vec4( 0.0, 1.0, 0.0, 1.0 ) * (alpha));
			facePoints.push_back({ p2.x, p2.y, p2.z, 1.0 });
			facePoints.push_back(vec4(0.0, 1.0, 0.0, 1.0) * (alpha));
			facePoints.push_back({centroid.x, centroid.y, centroid.z, 1.0});
			facePoints.push_back(vec4(0.0, 1.0, 0.0, 1.0) * (alpha));
		}
	}

	/* For sample points */
	samplePointsVBOSize = samplePoints.size() * sizeof(cl_float4);
	glDeleteBuffers(1, &samplePointsVBO);
	glGenBuffers(1, &samplePointsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, samplePointsVBO);
	glBufferData(GL_ARRAY_BUFFER, samplePointsVBOSize, samplePoints.data(), GL_DYNAMIC_DRAW);

	/* For edge points/faces */
	edgePointsVBOSize = facePoints.size() * sizeof(cl_float4);
	glDeleteBuffers(1, &edgePointsVBO);
	glGenBuffers(1, &edgePointsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, edgePointsVBO);
	glBufferData(GL_ARRAY_BUFFER, edgePointsVBOSize, facePoints.data(), GL_DYNAMIC_DRAW);
}

void SlicedVolume::updateVAO() {
	/* For sample points*/
	glDeleteVertexArrays(1, &samplePointsVAO);
	glGenVertexArrays(1, &samplePointsVAO);
	glBindVertexArray(samplePointsVAO);
	glBindBuffer(GL_ARRAY_BUFFER, samplePointsVBO);

	glEnableVertexAttribArray(Shaders::pointProgram->position_id);
	glEnableVertexAttribArray(Shaders::pointProgram->color_id);
	glVertexAttribPointer(Shaders::pointProgram->position_id, 4,
		GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glVertexAttribPointer(Shaders::pointProgram->color_id, 4,
		GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));


	/* For edge points */
	glDeleteVertexArrays(1, &edgePointsVAO);
	glGenVertexArrays(1, &edgePointsVAO);
	glBindVertexArray(edgePointsVAO);
	glBindBuffer(GL_ARRAY_BUFFER, edgePointsVBO);

	glEnableVertexAttribArray(Shaders::pointProgram->position_id);
	glEnableVertexAttribArray(Shaders::pointProgram->color_id);
	glVertexAttribPointer(Shaders::pointProgram->position_id, 4,
		GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glVertexAttribPointer(Shaders::pointProgram->color_id, 4,
		GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));

}

void SlicedVolume::render(glm::mat4 parent_matrix) {
	updateVBO();
	updateVAO();
	glm::mat4 finalMatrix = parent_matrix * M;

	Shaders::pointProgram->use();


	/* Edge points*/
	if (renderEdgePoints || renderFaces) {
		glBindVertexArray(edgePointsVAO);
		glBindBuffer(GL_ARRAY_BUFFER, edgePointsVBO);
		glUniformMatrix4fv(Shaders::pointProgram->matrix_id, 1, 0, &(finalMatrix[0].x));
		glUniform1f(Shaders::pointProgram->pointSize_id, 10.0);
		if (renderEdgePoints)
			glDrawArrays(GL_POINTS, 0, edgePointsVBOSize / sizeof(float4));
		if (renderFaces)
			glDrawArrays(GL_TRIANGLES, 0, edgePointsVBOSize / sizeof(float4));
	}

	/* Sample points */
	if (renderSamplePoints) {
		glBindVertexArray(samplePointsVAO);
		glBindBuffer(GL_ARRAY_BUFFER, samplePointsVBO);
		glUniformMatrix4fv(Shaders::pointProgram->matrix_id, 1, 0, &(finalMatrix[0].x));
		glUniform1f(Shaders::pointProgram->pointSize_id, 10.0);
		glDrawArrays(GL_POINTS, 0, samplePointsVBOSize / sizeof(float4));
		glDrawArrays(GL_LINE_STRIP, 0, samplePointsVBOSize / sizeof(float4));
	}

	if (renderSlices) {
		Shaders::slicedVolProgram->use();
		glBindVertexArray(edgePointsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glUniformMatrix4fv(Shaders::slicedVolProgram->matrix_id, 1, 0, &(finalMatrix[0].x));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, textureID);
		glUniform1i(Shaders::slicedVolProgram->texture0_id, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, transferFunction->getTextureID());
		glUniform1i(Shaders::slicedVolProgram->texture1_id, 1);


		glDrawArrays(GL_TRIANGLES, 0, edgePointsVBOSize / sizeof(float4));
		print_gl_error();
	}
}

void SlicedVolume::handleKeys()
{
	if (glfwGetKey(GLUtilities::window, GLFW_KEY_Q)) {
		renderSamplePoints = true;
	}

	if (glfwGetKey(GLUtilities::window, GLFW_KEY_A)) {
		renderEdgePoints = true;
	}

	if (glfwGetKey(GLUtilities::window, GLFW_KEY_W)) {
		renderFaces = true;
	}

	if (glfwGetKey(GLUtilities::window, GLFW_KEY_S)) {
		renderSlices = true;
	}

	if (glfwGetKey(GLUtilities::window, GLFW_KEY_R)) {
		renderEdgePoints = renderSamplePoints = renderFaces = renderSlices = false;
	}

	if (glfwGetKey(GLUtilities::window, GLFW_KEY_Y)) {
		samplesToTake = 16;
	}

	if (glfwGetKey(GLUtilities::window, GLFW_KEY_U)) {
		samplesToTake = 32;
	}

	if (glfwGetKey(GLUtilities::window, GLFW_KEY_I)) {
		samplesToTake = 64;
	}

	if (glfwGetKey(GLUtilities::window, GLFW_KEY_O)) {
		samplesToTake = 128;
	}

	if (glfwGetKey(GLUtilities::window, GLFW_KEY_P)) {
		samplesToTake = 256;
	}
}

void SlicedVolume::update() {
	handleKeys();
}

void SlicedVolume::updateImage(string filename, int3 rawDimensions, int bytesPerPixel) {
	/* Loads and uploads a 3D texture to the GPU */
	const int sizeInBytes = rawDimensions.x * rawDimensions.y * rawDimensions.z * bytesPerPixel;
	FILE *pFile = fopen(filename.c_str(), "rb");
	if (NULL == pFile) {
		/*return false;*/
		cout << "ERROR LOADING VOLUME " << filename << endl;
		return;
	}
		
	vector<GLubyte> pVolume(rawDimensions.x * rawDimensions.y * rawDimensions.z * bytesPerPixel);
	fread(pVolume.data(), sizeof(GLubyte), sizeInBytes, pFile);
	fclose(pFile);

	//int comp;
	//int forceChannels = 4;
	//stbi_set_flip_vertically_on_load(1);
	//unsigned char* image = stbi_load(imagePath.c_str(), &width, &height, &comp, forceChannels);
	//assert(image != 0);

	glGenTextures(1, &textureID);
	print_gl_error();

	glBindTexture(GL_TEXTURE_3D, textureID);
	print_gl_error();

	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, rawDimensions.x, rawDimensions.y, rawDimensions.z, 0, GL_RED, (bytesPerPixel == 1) ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT, pVolume.data());
	print_gl_error();
}