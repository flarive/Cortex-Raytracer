#include "aabb_debug.h"

#include "../materials/lambertian.h"

std::shared_ptr<box> aabb_debug::aabb_to_box_primitive(const aabb& bbox)
{
	auto xmin = bbox.x.min; // -2.7
	auto xmax = bbox.x.max; // -1.7
	auto ymin = bbox.y.min; // -0.5
	auto ymax = bbox.y.max; // 0.5
	auto zmin = bbox.z.min; // -1.5
	auto zmax = bbox.z.max; // -0.5

	std::shared_ptr<lambertian> debug_material = std::make_shared<lambertian>(color(0.5, 0.5, 0.5), 0.0, 1.0);

	auto center = vector3((xmin + xmax) / 2.0, (ymin + ymax) / 2.0, (zmin + zmax) / 2.0);
	auto size = vector3(xmax - xmin, ymax - ymin, zmax - zmin);
	return std::make_shared<box>(center, size, debug_material, "DebugBox");
}