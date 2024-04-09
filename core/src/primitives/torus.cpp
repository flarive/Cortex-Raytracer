#include "torus.h"

#include <glm/glm.hpp>




torus::torus(std::string _name) : center(0, 0, 0), majorRadius(0.5), minorRadius(0.25)
{
    name = _name;

    vector3 maxPoint = center + vector3(majorRadius + minorRadius, majorRadius + minorRadius, majorRadius + minorRadius);
    vector3 minPoint = center - vector3(majorRadius + minorRadius, majorRadius + minorRadius, majorRadius + minorRadius);
    bbox = aabb(minPoint, maxPoint);
}

torus::torus(vector3 center, float _majorRadius, float _minorRadius, std::shared_ptr<material> _material, std::string _name)
	: center(center), majorRadius(_majorRadius), minorRadius(_minorRadius), mat(_material)
{
	name = _name;

    vector3 maxPoint = center + vector3(majorRadius + minorRadius, majorRadius + minorRadius, majorRadius + minorRadius);
    vector3 minPoint = center - vector3(majorRadius + minorRadius, majorRadius + minorRadius, majorRadius + minorRadius);
    bbox = aabb(minPoint, maxPoint);
}



bool torus::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    vector3 oc = r.origin() - center;
    double a = dot(r.direction(), r.direction());
    double b = 2.0 * dot(oc, r.direction());
    double c = dot(oc, oc) - minorRadius * minorRadius;
    double d = b * b - 4 * a * c;

    if (d > 0) {
        double root = sqrt(d);
        double t = (-b - root) / (2.0 * a);
        if (t < ray_t.max && t > ray_t.min) {
            vector3 hit_point = r.at(t);
            if (bbox.hit(r, ray_t)) {
                vector3 hit_normal = normal(hit_point);
                rec.set_face_normal(r, hit_normal);
                rec.t = t;
                rec.hit_point = hit_point;
                rec.u = calculateU(hit_point);
                rec.v = calculateV(hit_point);
                rec.mat = mat;
                return true;
            }
        }
        t = (-b + root) / (2.0 * a);
        if (t < ray_t.max && t > ray_t.min) {
            vector3 hit_point = r.at(t);
            if (bbox.hit(r, ray_t)) {
                vector3 hit_normal = normal(hit_point);
                rec.set_face_normal(r, hit_normal);
                rec.t = t;
                rec.hit_point = hit_point;
                rec.u = calculateU(hit_point);
                rec.v = calculateV(hit_point);
                rec.mat = mat;
                return true;
            }
        }
    }
    return false;
}

vector3 torus::normal(const vector3& point) const
{
    vector3 outward_normal = unit_vector(point - center);
    double u = glm::dot(outward_normal, axis);
    return unit_vector(outward_normal - u * axis);
}

aabb torus::bounding_box() const
{
	return bbox;
}



double torus::calculateU(const vector3& point) const
{
    vector3 n = normal(point);
    vector3 u_axis = glm::cross(axis, n);
    return atan2(glm::dot(u_axis, glm::cross(vector3(0, 1, 0), axis)), glm::dot(u_axis, axis)) / (2 * M_PI) + 0.5;
}

double torus::calculateV(const vector3& point) const
{
    vector3 n = normal(point);
    vector3 v_axis = glm::cross(n, axis);
    return atan2(dot(v_axis, glm::cross(vector3(0, 1, 0), n)), glm::dot(v_axis, n)) / (2 * M_PI) + 0.5;
}

/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void torus::updateBoundingBox()
{
	// to implement
}