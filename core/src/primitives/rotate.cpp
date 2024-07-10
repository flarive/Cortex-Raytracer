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
    m_name = _p->getName();

    m_object = _p;
    // Convert Euler angles to radians
    glm::vec3 eulerRadians = glm::radians(_rotation);

    // Create quaternions for each axis rotation
    glm::quat quaternionX = glm::angleAxis(eulerRadians.x, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotation around X-axis
    glm::quat quaternionY = glm::angleAxis(eulerRadians.y, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation around Y-axis
    glm::quat quaternionZ = glm::angleAxis(eulerRadians.z, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotation around Z-axis

    // Combine quaternions into a single rotation quaternion
    m_rotationQuaternion = quaternionZ * quaternionY * quaternionX; // Ensure correct order

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
        rec.normal = glm::normalize(m_rotationQuaternion * rec.normal); // Ensure normal remains normalized

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
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void rt::rotate::updateBoundingBox()
{
    // to implement
}
