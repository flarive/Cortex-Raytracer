#include "rotate.h"
#include <limits.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../utilities/math_utils.h"

rt::rotate::rotate(std::shared_ptr<hittable> _object, const vector3& _rotation) : m_object(_object), m_rotation(_rotation)
{
    m_name = _object->getName();

    auto radians_x = degrees_to_radians(_rotation.x);
    auto radians_y = degrees_to_radians(_rotation.y);
    auto radians_z = degrees_to_radians(_rotation.z);


    matrix4 rotationMatrix(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, radians_x, vector3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, radians_y, vector3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, radians_z, vector3(0.0f, 0.0f, 1.0f));

    bbox = m_object->bounding_box();

    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                vector4 corner(
                    i * bbox.x.max + (1 - i) * bbox.x.min,
                    j * bbox.y.max + (1 - j) * bbox.y.min,
                    k * bbox.z.max + (1 - k) * bbox.z.min,
                    1.0f
                );

                vector4 rotatedCorner = rotationMatrix * corner;
                vector3 tester(rotatedCorner.x, rotatedCorner.y, rotatedCorner.z);

                for (int c = 0; c < 3; c++) {
                    min[c] = fmin(min[c], tester[c]);
                    max[c] = fmax(max[c], tester[c]);
                }
            }
        }
    }

    bbox = aabb(min, max);
}

bool rt::rotate::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    // Change the ray from world space to object space
    auto origin = r.origin();
    auto direction = r.direction();

    vector4 origin_vec(origin[0], origin[1], origin[2], 1.0f);
    vector4 direction_vec(direction[0], direction[1], direction[2], 0.0f);

    auto radians_x = degrees_to_radians(-m_rotation.x);
    auto radians_y = degrees_to_radians(-m_rotation.y);
    auto radians_z = degrees_to_radians(-m_rotation.z);

    matrix4 inverseRotationMatrix(1.0f);
    inverseRotationMatrix = glm::rotate(inverseRotationMatrix, radians_x, vector3(1.0f, 0.0f, 0.0f));
    inverseRotationMatrix = glm::rotate(inverseRotationMatrix, radians_y, vector3(0.0f, 1.0f, 0.0f));
    inverseRotationMatrix = glm::rotate(inverseRotationMatrix, radians_z, vector3(0.0f, 0.0f, 1.0f));

    vector4 rotated_origin = inverseRotationMatrix * origin_vec;
    vector4 rotated_direction = inverseRotationMatrix * direction_vec;

    ray rotated_r(point3(rotated_origin.x, rotated_origin.y, rotated_origin.z),
        vector3(rotated_direction.x, rotated_direction.y, rotated_direction.z),
        r.time());

    // Determine whether an intersection exists in object space (and if so, where)
    if (!m_object->hit(rotated_r, ray_t, rec, depth))
        return false;

    // Change the intersection point from object space to world space
    vector4 hit_point_vec(rec.hit_point[0], rec.hit_point[1], rec.hit_point[2], 1.0f);
    vector4 normal_vec(rec.normal[0], rec.normal[1], rec.normal[2], 0.0f);

    matrix4 rotationMatrix(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, degrees_to_radians(m_rotation.x), vector3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, degrees_to_radians(m_rotation.y), vector3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, degrees_to_radians(m_rotation.z), vector3(0.0f, 0.0f, 1.0f));

    vector4 world_hit_point = rotationMatrix * hit_point_vec;
    vector4 world_normal = rotationMatrix * normal_vec;

    rec.hit_point = point3(world_hit_point.x, world_hit_point.y, world_hit_point.z);
    rec.normal = vector3(world_normal.x, world_normal.y, world_normal.z);

    return true;
}

aabb rt::rotate::bounding_box() const
{
    return bbox;
}

void rt::rotate::updateBoundingBox()
{
    // to implement
}