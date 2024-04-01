#pragma once

#include "../primitives/hittable.h"
#include "../primitives/hittable_list.h"
#include "../cameras/camera_base.h"
#include "../lights/light.h"

#include <memory>
#include <vector>

class scene
{
public:
	scene();
	scene(const camera_base& _camera);
	void add(std::shared_ptr<hittable> _object);
	void set_camera(std::shared_ptr<camera_base> _camera);

	const hittable_list& get_world();
	const hittable_list& get_lights();
	std::shared_ptr<camera_base> get_camera();

	void extract_lights();
	void build_optimized_world();

private:
	hittable_list world;
	std::shared_ptr<camera_base> cam;
	hittable_list lights;
};