#include "scene.h"

#include "../misc/bvh_node.h"

#include "../primitives/box.h"
#include "../primitives/sphere.h"
#include "../lights/light.h"

#include "../misc/singleton.h"


scene::scene()
{
}

void scene::set(const hittable_list& _objects)
{
	m_world = _objects;
}

void scene::add(std::shared_ptr<hittable> _object)
{
	m_world.add(_object); // std::move ????????
}



void scene::set_camera(std::shared_ptr<camera> _camera)
{
	m_camera = _camera;
}

void scene::build_optimized_world(randomizer& rnd)
{
	// calculate bounding boxes to speed up ray computing
	m_world = hittable_list(std::make_shared<bvh_node>(m_world, rnd));
}

const hittable_list& scene::get_world()
{
	return m_world;
}

void scene::extract_emissive_objects()
{
	m_emissive_objects.clear();

	for (unsigned int i = 0; i < m_world.objects.size(); i++)
	{
		std::shared_ptr<light> derived = std::dynamic_pointer_cast<light>(m_world.objects[i]);
		if (derived)
		{
			m_emissive_objects.add(derived);
		}
	}
}

const hittable_list& scene::get_emissive_objects()
{
	return m_emissive_objects;
}

std::shared_ptr<camera> scene::get_camera()
{
	return m_camera;
}