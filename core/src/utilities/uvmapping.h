#pragma once

#include "types.h"


class uvmapping
{
public:
	uvmapping();
	uvmapping(double scale_u, double scale_v);
	uvmapping(double scale_u, double scale_v, double offset_u, double offset_v);

	double scale_u() const;
	double scale_v() const;

	double offset_u() const;
	double offset_v() const;

private:
	double m_scale_u = 1.0;
	double m_scale_v = 1.0;
	double m_offset_u = 0.0;
	double m_offset_v = 0.0;
};

extern void get_sphere_uv(const point3& p, double& u, double& v, const uvmapping& mapping);
extern void get_torus_uv(const vector3& _p, vector3& _c, double& _u, double& _v, double _majorRadius, double _minorRadius, const uvmapping& mapping);
extern void get_cylinder_uv(const vector3& p, double& u, double& v, double radius, double height, const uvmapping& mapping);
extern void get_disk_uv(const vector3& p, double& u, double& v, double radius, const uvmapping& mapping);
extern void get_cone_uv(const vector3& p, double& u, double& v, double radius, double height, const uvmapping& mapping);

extern void get_xy_rect_uv(double x, double y, double& u, double& v, float x0, float x1, float y0, float y1, const uvmapping& mapping);
extern void get_xz_rect_uv(double x, double y, double& u, double& v, float x0, float x1, float y0, float y1, const uvmapping& mapping);
extern void get_yz_rect_uv(double y, double z, double& u, double& v, float y0, float y1, float z0, float z1, const uvmapping& mapping);

extern void get_triangle_uv(const vector3 hitpoint, double& u, double& v, const vector3 verts[3], const vector2 vert_uvs[3]);
extern vector2 calculateTextureCoordinate(vector2 uv0, vector2 uv1, vector2 uv2, const vector2& barycentricCoords);