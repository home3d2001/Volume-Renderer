//#include "./Scene.h"
//using namespace std;
//using namespace glm;
//
//Entity Scene_OLD::root = Entity();
//
//void Scene_OLD::add(const string key, shared_ptr<Entity> object) {
//	root.objects[key] = object;
//}
//
//void Scene_OLD::remove(const string key) {
//	root.objects.erase(key);
//}
//
//void Scene_OLD::toggle(const std::string key) {
//	root.objects.at(key).get()->toggled = !root.objects.at(key).get()->toggled;
//}
//
//void Scene_OLD::move(const string key, mat4 matrix) {
//	root.objects[key]->matrix = matrix;
//}
//
//void Scene_OLD::render(mat4 matrix) {
//	for (auto i : root.objects) {
//		if (i.second.get()->toggled && i.second.get()->renderable)
//			i.second->render(matrix);
//	}
//}