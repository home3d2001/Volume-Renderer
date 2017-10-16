#include "Tests/catch.hpp"
#include "Tests/HelperFunctions.hpp"
#include "GLUtilities/gl_utils.h"
#include "GLUtilities/Scene.h"
#include "GLUtilities/Polylines/Polylines.hpp"

#include <memory>
void bench1() {
	using namespace std;
	glfwShowWindow(GLUtilities::window);

	/* Setup Scene*/
	shared_ptr<PolyLines> polylines = make_shared<PolyLines>();
	shared_ptr<Quadtree> quadtree = make_shared<Quadtree>();
	Scene::add("quadtree", quadtree);
	Scene::add("lines", polylines);
	Options::pruneOctree = false;

	glm::vec2 offsets[4] = { { -.8f, -.8f },{ .8f, -.8f },{ .8f, .8f },{ -.8f, .8f } };
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glDisable(GL_DEPTH_TEST);
	for (int n = 10000; n <= 1000000; n *= 10) {
		polylines->clear();
		int width = ceil(n / 4.f);
		for (int i = 0; i < n; i++) {
			int location = i % width;
			int side = i / width;
			glm::vec2 A = offsets[side];
			glm::vec2 B = offsets[(side + 1) % 4];
			float t1 = ((float)location) / (width);
			float t2 = (((float)location + 1) / (width)) * .99;
			glm::vec2 p1 = t1 * A + (1.0f - t1) * B;
			glm::vec2 p2 = t2 * A + (1.0f - t2) * B;
			polylines->newLine(make_float2(p1.x, p1.y));
			polylines->addPoint(make_float2(p2.x, p2.y));
		}
//		polylines->write2DToFile("./Scalability/" + std::to_string(n));
		cout << "n" << n << endl;
		quadtree->build(polylines.get());
		quadtree->build(polylines.get());
		quadtree->build(polylines.get());
		quadtree->build(polylines.get());
		quadtree->build(polylines.get());
		quadtree->build(polylines.get());
		quadtree->build(polylines.get());
		quadtree->build(polylines.get());
		quadtree->build(polylines.get());
		quadtree->build(polylines.get());
		quadtree->build(polylines.get());
		quadtree->build(polylines.get());

		cout << endl;
		cout << endl;
		/*cout << "" << endl;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Scene::render();
		glfwSwapBuffers(GLUtilities::window);
		glfwPollEvents();*/
	}

	//for (int i = 0; i < 1000; ++i) {
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	vector<float3> line;
	//	line.push_back(make_float3(i / 1000.f, i / 1000.f, i / 1000.f));
	//	line.push_back(make_float3((i + 1) / 1000.f, (i + 1) / 1000.f, (i + 1) / 1000.f));
	//	polylines->addLine(line);
	//	Scene::render();
	//	glfwSwapBuffers(GLUtilities::window);
	//}

	glfwHideWindow(GLUtilities::window);
}

void bench2() {
	using namespace std;
	glfwShowWindow(GLUtilities::window);

	/* Setup Scene*/
	shared_ptr<PolyLines> polylines = make_shared<PolyLines>();
	shared_ptr<Quadtree> quadtree = make_shared<Quadtree>();
	Scene::add("quadtree", quadtree);
	Scene::add("lines", polylines);
	Options::pruneOctree = false;

	glm::vec2 offsets[4] = { { -.8f, -.8f },{ .8f, -.8f },{ .8f, .8f },{ -.8f, .8f } };


	int n = 100000;
	polylines->clear();
	int width = ceil(n / 4.f);
	for (int i = 0; i < n; i++) {
		int location = i % width;
		int side = i / width;
		glm::vec2 A = offsets[side];
		glm::vec2 B = offsets[(side + 1) % 4];
		float t1 = ((float)location) / (width);
		float t2 = (((float)location + 1) / (width)) * .99;
		glm::vec2 p1 = t1 * A + (1.0f - t1) * B;
		glm::vec2 p2 = t2 * A + (1.0f - t2) * B;
		polylines->newLine(make_float2(p1.x, p1.y));
		polylines->addPoint(make_float2(p2.x, p2.y));
	}
	//		polylines->write2DToFile("./Scalability/" + std::to_string(n));
	cout << "n" << n << endl;
	quadtree->build(polylines.get());
	quadtree->build(polylines.get());
	quadtree->build(polylines.get());
	quadtree->build(polylines.get());
	quadtree->build(polylines.get());
	quadtree->build(polylines.get());
	quadtree->build(polylines.get());
	quadtree->build(polylines.get());
	quadtree->build(polylines.get());
	quadtree->build(polylines.get());
	quadtree->build(polylines.get());
	quadtree->build(polylines.get());
}

Benchmark("increasing number of facets", "[disabled][gears]") {
	bench1();
}

Benchmark("Increasing cores on facets", "[disabled][gears]") {
	bench2();
}