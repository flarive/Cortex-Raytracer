#pragma once

#include "../primitives/hittable.h"
#include "../primitives/hittable_list.h"
#include "../lights/light.h"

#include <memory>
#include <vector>

class camera;

class scene
{
public:
	scene();
	scene(std::shared_ptr<camera> _camera);
	void add(std::shared_ptr<hittable> _object);
	void set_camera(std::shared_ptr<camera> _camera);

	const hittable_list& get_world();
	const hittable_list& get_lights();
	std::shared_ptr<camera> get_camera();

	void extract_lights();
	void build_optimized_world();

private:
	hittable_list world;
	std::shared_ptr<camera> cam;
	hittable_list lights;
};