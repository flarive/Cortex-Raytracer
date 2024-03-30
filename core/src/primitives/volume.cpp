#include "volume.h"



bool volume::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    // Print occasional samples when debugging. To enable, set enableDebug true.
    const bool enableDebug = false;
    const bool debugging = enableDebug && random_double() < 0.00001;

    hit_record rec1, rec2;

    if (!boundary->hit(r, interval::universe, rec1, depth))
        return false;

    if (!boundary->hit(r, interval(rec1.t + 0.0001, infinity), rec2, depth))
        return false;

    if (debugging) std::clog << "\nray_tmin=" << rec1.t << ", ray_tmax=" << rec2.t << '\n';

    if (rec1.t < ray_t.min) rec1.t = ray_t.min;
    if (rec2.t > ray_t.max) rec2.t = ray_t.max;

    if (rec1.t >= rec2.t)
        return false;

    if (rec1.t < 0)
        rec1.t = 0;

    auto ray_length = vector_length(r.direction());// .length(); ??????????
    auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    auto hit_distance = neg_inv_density * log(random_double());

    if (hit_distance > distance_inside_boundary)
        return false;

    rec.t = rec1.t + hit_distance / ray_length;
    rec.hit_point = r.at(rec.t);

    if (debugging) {
        std::clog << "hit_distance = " << hit_distance << '\n'
            << "rec.t = " << rec.t << '\n'
            << "rec.hit_point = " << rec.hit_point << '\n';
    }

    rec.normal = vector3(1, 0, 0);  // arbitrary
    rec.front_face = true;     // also arbitrary
    rec.mat = phase_function;
    rec.name = name;
    //rec.bbox = bbox;

    return true;
}

aabb volume::bounding_box() const
{
    return boundary->bounding_box();
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void volume::updateBoundingBox()
{
    // to implement
}