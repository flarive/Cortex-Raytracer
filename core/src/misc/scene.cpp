#include "scene.h"

#include "../bvh_node.h"

#include "../primitives/box.h"
#include "../primitives/sphere.h"
#include "../lights/light.h"


scene::scene()
{

}

//scene::scene(std::shared_ptr<camera> _camera)
//{
//	cam = _camera;
//}

void scene::set(const hittable_list& _objects)
{
	world = _objects;
}

void scene::add(std::shared_ptr<hittable> _object)
{
	world.add(_object); // std::move ????????
}



//void scene::set_camera(std::shared_ptr<camera> _camera)
//{
//	cam = _camera;
//}

void scene::build_optimized_world()
{
	// calculate bounding boxes to speed up ray computing
	world = hittable_list(std::make_shared<bvh_node>(world));
}

const hittable_list& scene::get_world()
{
	return world;
}

void scene::extract_lights()
{
	lights.clear();

	for (unsigned int i = 0; i < world.objects.size(); i++)
	{
		std::shared_ptr<light> derived = std::dynamic_pointer_cast<light>(world.objects[i]);
		if (derived)
		{
			lights.add(derived);
		}
	}
}

const hittable_list& scene::get_lights()
{
	return lights;
}

//std::shared_ptr<camera> scene::get_camera()
//{
//	return cam;
//}