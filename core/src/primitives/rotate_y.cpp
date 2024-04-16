#include "rotate_y.h"


raytracer::rotate_y::rotate_y(std::shared_ptr<hittable> p, double angle) : object(p)
{
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    bbox = object->bounding_box();

    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
                auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
                auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

                auto newx = cos_theta * x + sin_theta * z;
                auto newz = -sin_theta * x + cos_theta * z;

                vector3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
                    min[c] = fmin(min[c], tester[c]);
                    max[c] = fmax(max[c], tester[c]);
                }
            }
        }
    }

    bbox = aabb(min, max);
}


bool raytracer::rotate_y::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    // Change the ray from world space to object space
    auto origin = r.origin();
    auto direction = r.direction();

    origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
    origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

    direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
    direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

    ray rotated_r(origin, direction, r.time());

    // Determine where (if any) an intersection occurs in object space
    if (!object->hit(rotated_r, ray_t, rec, depth))
        return false;

    // Change the intersection point from object space to world space
    auto p = rec.hit_point;
    p[0] = cos_theta * rec.hit_point[0] + sin_theta * rec.hit_point[2];
    p[2] = -sin_theta * rec.hit_point[0] + cos_theta * rec.hit_point[2];

    // Change the normal from object space to world space
    auto normal = rec.normal;
    normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
    normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

    rec.hit_point = p;
    rec.normal = normal;

    return true;
}

aabb raytracer::rotate_y::bounding_box() const
{
    return bbox;
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void raytracer::rotate_y::updateBoundingBox()
{
    // to implement
}