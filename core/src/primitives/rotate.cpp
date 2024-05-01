#include "rotate.h"

#include <limits.h>
#include <glm/ext/matrix_transform.hpp>


/**
	* @param p object to rotate
	* @param angle rotation angle in degrees, + is CCW
	* @param _axis x = 0, y = 1, z = 2
	*/
rt::rotate::rotate(std::shared_ptr<hittable> _p, const vector3& _rotation)
{
	//m_object = _p;

	//double angle = 0.0;

	//if (_rotation.x != 0.0)
	//{
	//	m_axis = 0;
	//	angle = _rotation.x;
	//}
	//if (_rotation.y != 0.0)
	//{
	//	m_axis = 1;
	//	angle = _rotation.y;
	//}
	//if (_rotation.z != 0.0)
	//{
	//	m_axis = 2;
	//	angle = _rotation.z;
	//}

	//double radians = M_PI * angle / 180;
	//m_sin_theta = std::sin(radians);
	//m_cos_theta = std::cos(radians);

	//vector3 min = vector3(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	//vector3 max = vector3(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());
	//
	////iterate though each corner of box
	//for (int i = 0; i < 2; i++)
	//{
	//	for (int j = 0; j < 2; j++)
	//	{
	//		for (int k = 0; k < 2; k++)
	//		{
	//			auto x = i * m_bbox.x.max + (1 - i) * m_bbox.x.min;
	//			auto y = j * m_bbox.y.max + (1 - j) * m_bbox.y.min;
	//			auto z = k * m_bbox.z.max + (1 - k) * m_bbox.z.min;
	//			double newx = x, newy = y, newz = z;

	//			switch (m_axis)
	//			{
	//				//apply rotation
	//				case 0:
	//					newy = m_cos_theta * y + m_sin_theta * z;
	//					newz = -m_sin_theta * y + m_cos_theta * z;
	//					break;
	//				case 1:
	//					newx = m_cos_theta * x + m_sin_theta * z;
	//					newz = -m_sin_theta * x + m_cos_theta * z;
	//					break;
	//				case 2:
	//					newx = m_cos_theta * x + m_sin_theta * y;
	//					newy = -m_sin_theta * x + m_cos_theta * y;
	//					break;
	//			}

	//			vector3 tester = vector3(newx, newy, newz);
	//			for (int c = 0; c < 3; c++)
	//			{
	//				//find the max of the rotated box
	//				if (tester[c] > max[c])
	//				{
	//					max[c] = tester[c];
	//				}
	//				if (tester[c] < min[c])
	//				{
	//					min[c] = tester[c];
	//				}
	//			}
	//		}
	//	}
	//}

	//m_bbox = aabb(min, max);











	m_object = _p;

	//double radians_x = M_PI * _rotation.x / 180;
	//double radians_y = M_PI * _rotation.y / 180;
	//double radians_z = M_PI * _rotation.z / 180;

	//double cos_x = std::cos(radians_x);
	//double sin_x = std::sin(radians_x);
	//double cos_y = std::cos(radians_y);
	//double sin_y = std::sin(radians_y);
	//double cos_z = std::cos(radians_z);
	//double sin_z = std::sin(radians_z);

	//float angleDegrees = 45.0f;
	//float angleRadians = glm::radians(angleDegrees);

	//// Define the axis of rotation(in this case, rotating around the Y - axis)
	//glm::vec3 axisY(0.0f, 1.0f, 1.0f);

	//// Create a 3x3 rotation matrix
	//m_rotation_matrix = glm::mat3(glm::rotate(glm::mat4(1.0f), angleRadians, axisY));


	//aabb zzz = m_object->bounding_box();

	//// Transform the bounding box using the rotation matrix
	//vector3 min = vector3(zzz.x.min, zzz.y.min, zzz.z.min);
	//vector3 max = vector3(zzz.x.max, zzz.y.max, zzz.z.max);

	//vector3 new_min = m_rotation_matrix * min;
	//vector3 new_max = m_rotation_matrix * max;

	//// Create the rotated bounding box
	//m_bbox = aabb(min, max);





	// Convert Euler angles to radians
	glm::vec3 eulerRadians = glm::radians(_rotation);

	// Create quaternions for each axis rotation
	glm::quat quaternionX = glm::angleAxis(eulerRadians.x, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotation around X-axis
	glm::quat quaternionY = glm::angleAxis(eulerRadians.y, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation around Y-axis
	glm::quat quaternionZ = glm::angleAxis(eulerRadians.z, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotation around Z-axis

	// Combine quaternions into a single rotation quaternion
	m_rotationQuaternion = quaternionX * quaternionY * quaternionZ;

	// Calculate the bounding box of the original object
	aabb original_bbox = m_object->bounding_box();
	vector3 min = original_bbox.min();
	vector3 max = original_bbox.max();

	// Calculate the center of the bounding box
	m_center = (min + max) * vector3(0.5f);

	// Store half extents for later use
	m_halfExtents = (max - min) * vector3(0.5f);
}


bool rt::rotate::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
	//vector3 origin = r.origin();
	//vector3 direction = r.direction();

	//switch (m_axis)
	//{
	//	//unrotate
	//	case 0:
	//		origin.y = m_cos_theta * r.origin().y - m_sin_theta * r.origin().z;
	//		origin.z = m_sin_theta * r.origin().y + m_cos_theta * r.origin().z;
	//		direction.y = m_cos_theta * r.direction().y - m_sin_theta * r.direction().z;
	//		direction.z = m_sin_theta * r.direction().y + m_cos_theta * r.direction().z;
	//		break;
	//	case 1:
	//		origin.x = m_cos_theta * r.origin().x - m_sin_theta * r.origin().z;
	//		origin.z = m_sin_theta * r.origin().x + m_cos_theta * r.origin().z;
	//		direction.x = m_cos_theta * r.direction().x - m_sin_theta * r.direction().z;
	//		direction.z = m_sin_theta * r.direction().x + m_cos_theta * r.direction().z;
	//		break;
	//	case 2:
	//		origin.x = m_cos_theta * r.origin().x - m_sin_theta * r.origin().y;
	//		origin.y = m_sin_theta * r.origin().x + m_cos_theta * r.origin().y;
	//		direction.x = m_cos_theta * r.direction().x - m_sin_theta * r.direction().y;
	//		direction.y = m_sin_theta * r.direction().x + m_cos_theta * r.direction().y;
	//		break;
	//}

	//ray rotated_r = ray(origin, direction, r.time());
	//if (m_object->hit(rotated_r, ray_t, rec, depth))
	//{
	//	vector3 p = rec.hit_point;
	//	vector3 normal = rec.normal;

	//	switch (m_axis)
	//	{
	//		//rotate
	//		case 0:
	//			p.y = m_cos_theta * rec.hit_point.y + m_sin_theta * rec.hit_point.z;
	//			p.z = -m_sin_theta * rec.hit_point.y + m_cos_theta * rec.hit_point.z;
	//			normal.y = m_cos_theta * rec.normal.y + m_sin_theta * rec.normal.z;
	//			normal.z = -m_sin_theta * rec.normal.y + m_cos_theta * rec.normal.z;
	//			break;
	//		case 1:
	//			p.x = m_cos_theta * rec.hit_point.x + m_sin_theta * rec.hit_point.z;
	//			p.z = -m_sin_theta * rec.hit_point.x + m_cos_theta * rec.hit_point.z;
	//			normal.x = m_cos_theta * rec.normal.x + m_sin_theta * rec.normal.z;
	//			normal.z = -m_sin_theta * rec.normal.x + m_cos_theta * rec.normal.z;
	//			break;
	//		case 2:
	//			p.x = m_cos_theta * rec.hit_point.x + m_sin_theta * rec.hit_point.y;
	//			p.y = -m_sin_theta * rec.hit_point.x + m_cos_theta * rec.hit_point.y;
	//			normal.x = m_cos_theta * rec.normal.x + m_sin_theta * rec.normal.y;
	//			normal.y = -m_sin_theta * rec.normal.x + m_cos_theta * rec.normal.y;
	//			break;
	//	}
	//	rec.hit_point = p;
	//	rec.normal = normal;
	//	return true;
	//}

	//return false;







	//vector3 origin = r.origin();
	//vector3 direction = r.direction();

	//// Apply rotation to the ray's origin and direction
	//origin = m_rotation_matrix * origin;
	//direction = m_rotation_matrix * direction;

	//ray rotated_r = ray(origin, direction, r.time());
	//if (m_object->hit(rotated_r, ray_t, rec, depth))
	//{
	//	// Rotate back the hit point and normal
	//	rec.hit_point = glm::inverse(m_rotation_matrix) * rec.hit_point;
	//	auto mmm = glm::transpose(glm::inverse(m_rotation_matrix));
	//	rec.normal = mmm * rec.normal;

	//	return true;
	//}




	// Transform the ray origin and direction to be relative to the center of the primitive
	vector3 localOrigin = r.origin() - m_center;
	vector3 localDirection = r.direction();

	// Apply rotation to the ray's origin and direction
	vector3 rotatedOrigin = m_rotationQuaternion * localOrigin;
	vector3 rotatedDirection = m_rotationQuaternion * localDirection;

	// Create a transformed ray using the rotated origin and direction
	ray rotated_ray(rotatedOrigin + m_center, rotatedDirection, r.time());

	if (m_object->hit(rotated_ray, ray_t, rec, depth))
	{
		// Transform the hit point and normal back to the original world coordinates
		rec.hit_point = m_rotationQuaternion * (rec.hit_point - m_center) + m_center;
		rec.normal = m_rotationQuaternion * glm::normalize(rec.normal); // Ensure normal remains normalized

		return true;
	}

	return false;
}

aabb rt::rotate::bounding_box() const
{
	// Rotate the original bounding box corners
	vector3 corners[8];
	vector3 original_min = m_center - m_halfExtents;
	vector3 original_max = m_center + m_halfExtents;

	corners[0] = original_min;
	corners[1] = vector3(original_min.x, original_min.y, original_max.z);
	corners[2] = vector3(original_min.x, original_max.y, original_min.z);
	corners[3] = vector3(original_min.x, original_max.y, original_max.z);
	corners[4] = vector3(original_max.x, original_min.y, original_min.z);
	corners[5] = vector3(original_max.x, original_min.y, original_max.z);
	corners[6] = vector3(original_max.x, original_max.y, original_min.z);
	corners[7] = original_max;

	// Rotate each corner using the quaternion rotation
	vector3 rotated_corners[8];
	for (int i = 0; i < 8; ++i) {
		rotated_corners[i] = m_rotationQuaternion * (corners[i] - m_center) + m_center;
	}

	// Find the new minimum and maximum bounds after rotation
	vector3 new_min = rotated_corners[0];
	vector3 new_max = rotated_corners[0];
	for (int i = 1; i < 8; ++i) {
		new_min = glm::min(new_min, rotated_corners[i]);
		new_max = glm::max(new_max, rotated_corners[i]);
	}

	return aabb(new_min, new_max);
	
	//return m_bbox;
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void rt::rotate::updateBoundingBox()
{
	// to implement
}