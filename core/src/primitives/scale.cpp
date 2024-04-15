#include "scale.h"

scale::scale(std::shared_ptr<hittable> p, double f) : object(p), xf(f), yf(f), zf(f)
{
	bbox = object->bounding_box() * vector3(xf, yf, zf);
}

scale::scale(std::shared_ptr<hittable> p, double fx, double fy, double fz) : object(p), xf(fx), yf(fy), zf(fz)
{
	bbox = object->bounding_box() * vector3(xf, yf, zf);
}

bool scale::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
	vector3 origin = r.origin();
	origin = origin * vector3(1 / xf, 1 / yf, 1 / zf);

	ray scaled_r = ray(origin, r.direction(), r.time());
	if (object->hit(scaled_r, ray_t, rec, 0))
	{
		rec.hit_point = rec.hit_point * vector3(xf, yf, zf);
		return true;
	}

    return false;
}

aabb scale::bounding_box() const
{
    return bbox;
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void scale::updateBoundingBox()
{
    // to implement
}
