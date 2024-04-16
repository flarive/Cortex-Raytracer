#include "rotate.h"

#include <limits.h>

/**
	* @param p object to rotate
	* @param angle rotation angle in degrees, + is CCW
	* @param _axis x = 0, y = 1, z = 2
	*/
myOwnRaytracer::rotate::rotate(std::shared_ptr<hittable> _p, double _angle, int _axis)
{
	object = _p;
	axis = _axis;
	double radians = M_PI * _angle / 180;
	sin_theta = std::sin(radians);
	cos_theta = std::cos(radians);

	vector3 min = vector3(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	vector3 max = vector3(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());
	
	//iterate though each corner of box
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
				auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
				auto z = k * bbox.z.max + (1 - k) * bbox.z.min;
				double newx = x, newy = y, newz = z;

				switch (axis)
				{
					//apply rotation
					case 0:
						newy = cos_theta * y + sin_theta * z;
						newz = -sin_theta * y + cos_theta * z;
						break;
					case 1:
						newx = cos_theta * x + sin_theta * z;
						newz = -sin_theta * x + cos_theta * z;
						break;
					case 2:
						newx = cos_theta * x + sin_theta * y;
						newy = -sin_theta * x + cos_theta * y;
						break;
				}

				vector3 tester = vector3(newx, newy, newz);
				for (int c = 0; c < 3; c++)
				{
					//find the max of the rotated box
					if (tester[c] > max[c])
					{
						max[c] = tester[c];
					}
					if (tester[c] < min[c])
					{
						min[c] = tester[c];
					}
				}
			}
		}
	}

	bbox = aabb(min, max);
}


bool myOwnRaytracer::rotate::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
	vector3 origin = r.origin();
	vector3 direction = r.direction();

	switch (axis)
	{
		//unrotate
		case 0:
			origin.y = cos_theta * r.origin().y - sin_theta * r.origin().z;
			origin.z = sin_theta * r.origin().y + cos_theta * r.origin().z;
			direction.y = cos_theta * r.direction().y - sin_theta * r.direction().z;
			direction.z = sin_theta * r.direction().y + cos_theta * r.direction().z;
			break;
		case 1:
			origin.x = cos_theta * r.origin().x - sin_theta * r.origin().z;
			origin.z = sin_theta * r.origin().x + cos_theta * r.origin().z;
			direction.x = cos_theta * r.direction().x - sin_theta * r.direction().z;
			direction.z = sin_theta * r.direction().x + cos_theta * r.direction().z;
			break;
		case 2:
			origin.x = cos_theta * r.origin().x - sin_theta * r.origin().y;
			origin.y = sin_theta * r.origin().x + cos_theta * r.origin().y;
			direction.x = cos_theta * r.direction().x - sin_theta * r.direction().y;
			direction.y = sin_theta * r.direction().x + cos_theta * r.direction().y;
			break;
	}

	ray rotated_r = ray(origin, direction, r.time());
	if (object->hit(rotated_r, ray_t, rec, 0))
	{
		vector3 p = rec.hit_point;
		vector3 normal = rec.normal;

		switch (axis)
		{
			//rotate
			case 0:
				p.y = cos_theta * rec.hit_point.y + sin_theta * rec.hit_point.z;
				p.z = -sin_theta * rec.hit_point.y + cos_theta * rec.hit_point.z;
				normal.y = cos_theta * rec.normal.y + sin_theta * rec.normal.z;
				normal.z = -sin_theta * rec.normal.y + cos_theta * rec.normal.z;
				break;
			case 1:
				p.x = cos_theta * rec.hit_point.x + sin_theta * rec.hit_point.z;
				p.z = -sin_theta * rec.hit_point.x + cos_theta * rec.hit_point.z;
				normal.x = cos_theta * rec.normal.x + sin_theta * rec.normal.z;
				normal.z = -sin_theta * rec.normal.x + cos_theta * rec.normal.z;
				break;
			case 2:
				p.x = cos_theta * rec.hit_point.x + sin_theta * rec.hit_point.y;
				p.y = -sin_theta * rec.hit_point.x + cos_theta * rec.hit_point.y;
				normal.x = cos_theta * rec.normal.x + sin_theta * rec.normal.y;
				normal.y = -sin_theta * rec.normal.x + cos_theta * rec.normal.y;
				break;
		}
		rec.hit_point = p;
		rec.normal = normal;
		return true;
	}

	return false;
}

aabb myOwnRaytracer::rotate::bounding_box() const
{
	return bbox;
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void myOwnRaytracer::rotate::updateBoundingBox()
{
	// to implement
}
