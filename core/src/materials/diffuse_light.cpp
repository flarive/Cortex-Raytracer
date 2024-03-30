#include "diffuse_light.h"


diffuse_light::diffuse_light(std::shared_ptr<texture> a) : emit(a) {}

diffuse_light::diffuse_light(color _c) : emit(make_shared<solid_color_texture>(_c)), invisible(true), directional(true) {}

diffuse_light::diffuse_light(color _c, bool _directional, bool _invisible) : emit(make_shared<solid_color_texture>(_c)), directional(_directional), invisible(_invisible)
{

}

color diffuse_light::emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
{
    // Material emission, directional
    if (directional && !rec.front_face)
    {
        return invisible ? color(0, 0, 0, 0) : color(1, 1, 1, 0);
    }

    return emit->value(u, v, p);
}
