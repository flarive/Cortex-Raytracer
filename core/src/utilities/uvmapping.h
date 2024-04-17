#pragma once

#include "types.h"


class uvmapping
{
public:
	static void get_sphere_uv(const point3& p, double& u, double& v);
	static void get_torus_uv(const vector3& _p, vector3& _c, double& _u, double& _v, double _majorRadius, double _minorRadius);
	static void get_cylinder_uv(const vector3& p, double& u, double& v, double radius);
	static void get_disk_uv(const vector3& p, double& u, double& v, double radius);
	static void get_cone_uv(const vector3& p, double& u, double& v, double radius);

	static void get_xy_rect_uv(double x, double y, double& u, double& v, float x0, float x1, float y0, float y1);
};

