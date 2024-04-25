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

void scene::add(std::shared_ptr<hittable> _object)
{
	world.add(_object);
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


//void scene::add_debug_bounding_box()
//{
//	std::shared_ptr<lambertian> lambertian_material1 = std::make_shared<lambertian>(color(1.0, 0.1, 0.1), 0.5, 0.5);
//
//	for (unsigned int i = 0; i < world.objects.size(); i++)
//	{
//		auto zz = world.objects[i];
//		
//		add_debug_bounding_box_rec(zz, lambertian_material1);
//	}
//}

//void scene::add_debug_bounding_box_rec(std::shared_ptr<hittable> obj, std::shared_ptr<lambertian> mat)
//{
//	if (obj)
//	{
//		std::shared_ptr<bvh_node> derived = std::dynamic_pointer_cast<bvh_node>(obj);
//		if (derived)
//		{
//			aabb bbox = derived->bounding_box();
//			auto bounding_box = std::make_shared<box>(point3(bbox.x.min, bbox.y.min, bbox.z.min), point3(bbox.x.max, bbox.y.max, bbox.z.max), mat);
//			//world.add(bounding_box);
//
//			//derived->set_bounding_box(();
//		}
//	}
//}


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