#pragma once

#include "../primitives/hittable.h"
#include "../primitives/hittable_list.h"
//#include "../lights/SpotLight.h"

#include <memory>
#include <vector>

class camera;

class scene
{
public:
	scene();
	//scene(std::shared_ptr<camera> _camera);
	void set(const hittable_list& _objects);
	void add(std::shared_ptr<hittable> _object);
	//void set_camera(std::shared_ptr<camera> _camera);

	const hittable_list& get_world();
	const hittable_list& get_emissive_objects();
	//std::shared_ptr<camera> get_camera();

	//const std::vector<SpotLight> get_lights();

	void extract_emissive_objects();
	void build_optimized_world();

private:
	hittable_list m_world;
	//std::shared_ptr<camera> cam;
	hittable_list m_emissive_objects;

	//std::vector<SpotLight> m_lights;
};