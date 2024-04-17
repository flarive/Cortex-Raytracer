#include "uvmapping.h"

#include "../constants.h"


void uvmapping::get_sphere_uv(const point3& p, double& u, double& v)
{
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

    auto theta = acos(-p.y);
    auto phi = atan2(-p.z, p.x) + M_PI;

    u = phi / (2 * M_PI);
    v = theta / M_PI;
}

void uvmapping::get_torus_uv(const vector3& _p, vector3& _c, double& _u, double& _v, double _majorRadius, double _minorRadius)
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

void uvmapping::get_cylinder_uv(const vector3& p, double& u, double& v, double radius)
{
	auto theta = std::atan2(p[0], p[2]);
	auto phi = std::atan2(p[1], radius);
	u = 1 - (theta + M_PI) / (2 * M_PI);
	v = (phi + M_PI / 2) / M_PI;
}

void uvmapping::get_disk_uv(const vector3& p, double& u, double& v, double radius)
{
	auto theta = std::atan2(p[0], p[2]);
	auto phi = std::atan2(p[1], radius);
	u = 1 - (theta + M_PI) / (2 * M_PI);
	v = (phi + M_PI / 2) / M_PI;
}

void uvmapping::get_cone_uv(const vector3& p, double& u, double& v, double radius)
{
	/*auto theta = std::atan2(p.x, p.z);
	auto phi = std::atan2(p.y, radius);
	u = 1 - (theta + M_PI) / (2 * M_PI);
	v = (phi + M_PI / 2) / M_PI;*/

	auto theta = std::atan2(p.z, p.x);
	auto phi = std::atan2(p.y, radius);
	u = 1 - (theta + M_PI) / (2 * M_PI);
	v = phi / (2 * M_PI); // Linear mapping of phi to [0, 1]
}

void uvmapping::get_xy_rect_uv(double x, double y, double& u, double& v, float x0, float x1, float y0, float y1)
{
	u = (x - x0) / (x1 - x0);
	v = (y - y0) / (y1 - y0);

	// Apply texture scaling and offset to improve texture mapping
	double u_scale = 0.5; // Adjust as needed
	double v_scale = 0.5; // Adjust as needed
	double u_offset = 0.0; // Adjust as needed
	double v_offset = 0.0; // Adjust as needed

	u = u_scale * u + u_offset;
	v = v_scale * v + v_offset;
}


