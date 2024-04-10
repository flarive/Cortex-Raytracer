#include "torus.h"

#include <glm/glm.hpp>
#include <eigen/unsupported/eigen/polynomials>

#include <algorithm>


torus::torus(std::string _name) : center(0, 0, 0), _R(0.5), _r(0.25)
{
    name = _name;

	// calculate cylinder bounding box for ray optimizations
	double rR = _r + _R;
	o_b = aabb(
		point3(-rR, -rR, -_r),
		point3(rR, rR, _r)
	);

	_R2 = _R * _R;
	_R2r2 = _R2 - (_r * _r);
}

torus::torus(vector3 center, float _majorRadius, float _minorRadius, std::shared_ptr<material> _material, std::string _name)
	: center(center), _R(_majorRadius), _r(_minorRadius), mat(_material)
{
	name = _name;

	// calculate cylinder bounding box for ray optimizations
	double rR = _r + _R;
	o_b = aabb(
		point3(-rR, -rR, -_r),
		point3(rR, rR, _r)
	);

	_R2 = _R * _R;
	_R2r2 = _R2 - (_r * _r);
}

torus::torus(vector3 center, float _majorRadius, float _minorRadius, transformation* _transform, std::shared_ptr<material> _material, std::string _name) : center(center), _R(_majorRadius), _r(_minorRadius), m_transform(_transform), mat(_material)
{
	name = _name;

	// calculate cylinder bounding box for ray optimizations
	double rR = _r + _R;
	o_b = aabb(
		point3(-rR, -rR, -_r),
		point3(rR, rR, _r)
	);

	_R2 = _R * _R;
	_R2r2 = _R2 - (_r * _r);
}



bool torus::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
	//auto ss = r.origin();
	//auto nn = r.direction();

	//m_transform->transform(r.origin(), r.direction());

	//auto ss2 = r.origin();
	//auto nn2 = r.direction();

	
	//if (!hitbox(ray, t0, t1)) return false;
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
	vector3 c = glm::normalize(pp) * _R; // center of tube
	vector3 n = glm::normalize(p - c);
	rec.normal= n;
	rec.mat = mat;
	rec.hit_point = r.at(rec.t);
	rec.name = name;
	rec.bbox = o_b;

	//m_transform->inverse(rec.hit_point, rec.normal);


	return true;
}

//bool torus::hitbox(ray& r, double t0, double t1)
//{
//	double tmin, tmax, tymin, tymax, tzmin, tzmax;
//	vector3 s = r.sign;
//	vector3 i = r.inverseDirection();
//	vector3 e = r.origin();
//
//	tmin = (o_b.b[s[0]][0] - e[0]) * i[0];
//	tmax = (o_b.b[1 - s[0]][0] - e[0]) * i[0];
//	tymin = (o_b.b[s[1]][1] - e[1]) * i[1];
//	tymax = (o_b.b[1 - s[1]][1] - e[1]) * i[1];
//	if ((tmin > tymax) || (tymin > tmax)) return false;
//	if (tymin > tmin) tmin = tymin;
//	if (tymax < tmax) tmax = tymax;
//	tzmin = (o_b.b[s[2]][2] - e[2]) * i[2];
//	tzmax = (o_b.b[1 - s[2]][2] - e[2]) * i[2];
//	if ((tmin > tzmax) || (tzmin > tmax)) return false;
//	if (tzmin > tmin) tmin = tzmin;
//	if (tzmax < tmax) tmax = tzmax;
//	return ((tmin < t1) && (tmax > t0));
//}

aabb torus::bounding_box() const
{
	return o_b;
}

inline double pow2(const double& x) {
	return x * x;
}

inline double pow3(const double& x) {
	return pow2(x) * x;
}

inline double pow4(const double& x) {
	double p2 = pow2(x);
	return p2 * p2;
}


//double torus::calculateU(const vector3& point) const
//{
//    vector3 n = normal(point);
//    vector3 u_axis = glm::cross(axis, n);
//    return atan2(glm::dot(u_axis, glm::cross(vector3(0, 1, 0), axis)), glm::dot(u_axis, axis)) / (2 * M_PI) + 0.5;
//}
//
//double torus::calculateV(const vector3& point) const
//{
//    vector3 n = normal(point);
//    vector3 v_axis = glm::cross(n, axis);
//    return atan2(dot(v_axis, glm::cross(vector3(0, 1, 0), n)), glm::dot(v_axis, n)) / (2 * M_PI) + 0.5;
//}

/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void torus::updateBoundingBox()
{
	// to implement
}

