#include "data.h"


namespace Data_OLD {
	Polygons *polygons;
	std::shared_ptr<PolyLines> lines;
	std::shared_ptr<Boxes3D> boxes3D;
	std::shared_ptr<Boxes3D> LHand;
	std::shared_ptr<Boxes3D> RHand;
	std::shared_ptr<Quadtree> quadtree;
	std::shared_ptr<Octree> octree;
	std::shared_ptr<ImagePlane> instructions;
	vector<Gear> gears;
};
