#include "spot_light.h"


spot_light::spot_light(std::shared_ptr<texture>  a, vector3 dir, double cosTotalWidth, double cosFalloffStart, double intensity, bool invisible) :
    emit(a), spot_direction(unit_vector(dir)), intensity(intensity), cosTotalWidth(cosTotalWidth),
    cosFalloffStart(cosFalloffStart), invisible(invisible)
{

}

spot_light::~spot_light()
{

}

color spot_light::emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
{
    if (dot(rec.normal, r_in.direction()) < 0.0) {
        return(falloff(r_in.origin() - rec.hit_point) * emit->value(u, v, p) * intensity);
    }
    else {
        return(color(0, 0, 0));
    }
}

double spot_light::falloff(const vector3& w) const
{
    double cosTheta = dot(spot_direction, unit_vector(w));
    if (cosTheta < cosTotalWidth) {
        return(0);
    }
    if (cosTheta > cosFalloffStart) {
        return(1);
    }
    double delta = (cosTheta - cosTotalWidth) / (cosFalloffStart - cosTotalWidth);
    return((delta * delta) * (delta * delta));
}