#include "rotate.h"

#include <limits.h>

/**
	* @param p object to rotate
	* @param angle rotation angle in degrees, + is CCW
	* @param _axis x = 0, y = 1, z = 2
	*/
rt::rotate::rotate(std::shared_ptr<hittable> _p, const vector3& _rotation)
{
	m_object = _p;

	double angle = 0.0;

	if (_rotation.x != 0.0)
	{
		m_axis = 0;
		angle = _rotation.x;
	}
	if (_rotation.y != 0.0)
	{
		m_axis = 1;
		angle = _rotation.y;
	}
	if (_rotation.z != 0.0)
	{
		m_axis = 2;
		angle = _rotation.z;
	}

	double radians = M_PI * angle / 180;
	m_sin_theta = std::sin(radians);
	m_cos_theta = std::cos(radians);

	vector3 min = vector3(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	vector3 max = vector3(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());
	
	//iterate though each corner of box
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				auto x = i * m_bbox.x.max + (1 - i) * m_bbox.x.min;
				auto y = j * m_bbox.y.max + (1 - j) * m_bbox.y.min;
				auto z = k * m_bbox.z.max + (1 - k) * m_bbox.z.min;
				double newx = x, newy = y, newz = z;

				switch (m_axis)
				{
					//apply rotation
					case 0:
						newy = m_cos_theta * y + m_sin_theta * z;
						newz = -m_sin_theta * y + m_cos_theta * z;
						break;
					case 1:
						newx = m_cos_theta * x + m_sin_theta * z;
						newz = -m_sin_theta * x + m_cos_theta * z;
						break;
					case 2:
						newx = m_cos_theta * x + m_sin_theta * y;
						newy = -m_sin_theta * x + m_cos_theta * y;
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

	m_bbox = aabb(min, max);
}


bool rt::rotate::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
	vector3 origin = r.origin();
	vector3 direction = r.direction();

	switch (m_axis)
	{
		//unrotate
		case 0:
			origin.y = m_cos_theta * r.origin().y - m_sin_theta * r.origin().z;
			origin.z = m_sin_theta * r.origin().y + m_cos_theta * r.origin().z;
			direction.y = m_cos_theta * r.direction().y - m_sin_theta * r.direction().z;
			direction.z = m_sin_theta * r.direction().y + m_cos_theta * r.direction().z;
			break;
		case 1:
			origin.x = m_cos_theta * r.origin().x - m_sin_theta * r.origin().z;
			origin.z = m_sin_theta * r.origin().x + m_cos_theta * r.origin().z;
			direction.x = m_cos_theta * r.direction().x - m_sin_theta * r.direction().z;
			direction.z = m_sin_theta * r.direction().x + m_cos_theta * r.direction().z;
			break;
		case 2:
			origin.x = m_cos_theta * r.origin().x - m_sin_theta * r.origin().y;
			origin.y = m_sin_theta * r.origin().x + m_cos_theta * r.origin().y;
			direction.x = m_cos_theta * r.direction().x - m_sin_theta * r.direction().y;
			direction.y = m_sin_theta * r.direction().x + m_cos_theta * r.direction().y;
			break;
	}

	ray rotated_r = ray(origin, direction, r.time());
	if (m_object->hit(rotated_r, ray_t, rec, 0))
	{
		vector3 p = rec.hit_point;
		vector3 normal = rec.normal;

		switch (m_axis)
		{
			//rotate
			case 0:
				p.y = m_cos_theta * rec.hit_point.y + m_sin_theta * rec.hit_point.z;
				p.z = -m_sin_theta * rec.hit_point.y + m_cos_theta * rec.hit_point.z;
				normal.y = m_cos_theta * rec.normal.y + m_sin_theta * rec.normal.z;
				normal.z = -m_sin_theta * rec.normal.y + m_cos_theta * rec.normal.z;
				break;
			case 1:
				p.x = m_cos_theta * rec.hit_point.x + m_sin_theta * rec.hit_point.z;
				p.z = -m_sin_theta * rec.hit_point.x + m_cos_theta * rec.hit_point.z;
				normal.x = m_cos_theta * rec.normal.x + m_sin_theta * rec.normal.z;
				normal.z = -m_sin_theta * rec.normal.x + m_cos_theta * rec.normal.z;
				break;
			case 2:
				p.x = m_cos_theta * rec.hit_point.x + m_sin_theta * rec.hit_point.y;
				p.y = -m_sin_theta * rec.hit_point.x + m_cos_theta * rec.hit_point.y;
				normal.x = m_cos_theta * rec.normal.x + m_sin_theta * rec.normal.y;
				normal.y = -m_sin_theta * rec.normal.x + m_cos_theta * rec.normal.y;
				break;
		}
		rec.hit_point = p;
		rec.normal = normal;
		return true;
	}

	return false;
}

aabb rt::rotate::bounding_box() const
{
	return m_bbox;
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void rt::rotate::updateBoundingBox()
{
	// to implement
}