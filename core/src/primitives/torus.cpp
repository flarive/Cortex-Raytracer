#include "torus.h"

#include <glm/glm.hpp>
#include <eigen/unsupported/eigen/polynomials>

#include <algorithm>


torus::torus(std::string _name) : center(0, 0, 0), majorRadius(0.5), minorRadius(0.25)
{
    name = _name;

	// calculate torus bounding box for ray optimizations
	double rR = minorRadius + majorRadius;
	bbox = aabb(point3(-rR, -rR, -minorRadius), point3(rR, rR, minorRadius));

	_R2 = majorRadius * majorRadius;
	_R2r2 = _R2 - (minorRadius * minorRadius);
}

torus::torus(point3 center, float _majorRadius, float _minorRadius, std::shared_ptr<material> _material, std::string _name)
	: center(center), majorRadius(_majorRadius), minorRadius(_minorRadius), mat(_material)
{
	name = _name;

	// calculate torus bounding box for ray optimizations
	double rR = minorRadius + majorRadius;
	bbox = aabb(point3(-rR, -rR, -minorRadius), point3(rR, rR, minorRadius));

	_R2 = majorRadius * majorRadius;
	_R2r2 = _R2 - (minorRadius * minorRadius);
}

bool torus::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
	const vector3 d = r.direction();
	const vector3 e = r.origin() - center;

	double dx2 = d.x* d.x, dy2 = d.y * d.y;
	double ex2 = e.x* e.x, ey2 = e.y * e.y;
	double dxex = d.x* e.x, dyey = d.y * e.y;

	double A = glm::dot(d, d);
	double B = 2 * glm::dot(d, e);
	double C = glm::dot(e, e) + (_R2r2);
	double D = 4 *_R2* (dx2 + dy2);
	double E = 8 *_R2* (dxex + dyey);
	double F = 4 *_R2* (ex2 + ey2);

	Vector5d op;
	op << C * C - F, 2 * B * C - E, 2 * A * C + B * B - D, 2 * A * B, A* A;

	Eigen::PolynomialSolver<double, 4> psolve(op);
	std::vector<double> reals;
	psolve.realRoots(reals);

	for (int i = (int)reals.size() - 1; i >= 0; i--)
	{
		if (reals[i] < ray_t.min || reals[i] > ray_t.max)
		{
			reals.erase(reals.begin() + i);
		}
	}

	if ((size_t)reals.size() == 0)
	{
		return false;
	}

	std::sort(reals.begin(), reals.end());
	rec.t = reals[0];
	vector3 p = e + rec.t * d;
	vector3 pp = vector3(p.x, p.y, 0.);
	vector3 c = glm::normalize(pp) * majorRadius; // center of tube
	vector3 n = glm::normalize(p - c);

	rec.hit_point = r.at(rec.t);

	// set normal and front-face tracking
	vector3 outward_normal = (rec.hit_point - center) / majorRadius;
	rec.set_face_normal(r, outward_normal);


	// UV coordinates
	double u, v;
	get_torus_uv(p, c, u, v, majorRadius, minorRadius);

	// Set UV coordinates
	rec.u = u;
	rec.v = v;

	rec.mat = mat;
	rec.name = name;
	rec.bbox = bbox;

	return true;
}

aabb torus::bounding_box() const
{
	return bbox;
}

void torus::get_torus_uv(const vector3& _p, vector3& _c, double& _u, double& _v, double _majorRadius, double _minorRadius) const
{
	double phi = atan2(_p.y, _p.x);
	if (phi < 0) phi += 2 * M_PI; // Ensure phi is in [0, 2*pi]

	// Calculate the distance from the center of the torus in the xy-plane
	double dxy = glm::length(vector2(_p.x, _p.y) - vector2(_c.x, _c.y)) - _majorRadius;
	// Calculate the angle around the torus
	double theta = atan2(_p.z, dxy);
	if (theta < 0) theta += 2 * M_PI; // Ensure theta is in [0, 2*pi]

	// Normalize to [0, 1]
	double s = phi / (2 * M_PI);
	double t = theta / (2 * M_PI);


	// Apply texture scaling and offset to improve texture mapping
	double u_scale = 1.0; // Adjust as needed
	double v_scale = 1.0; // Adjust as needed
	double u_offset = 0.0; // Adjust as needed
	double v_offset = 0.0; // Adjust as needed

	_u = u_scale * s + u_offset;
	_v = v_scale * t + v_offset;
}

/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void torus::updateBoundingBox()
{
	// to implement
}