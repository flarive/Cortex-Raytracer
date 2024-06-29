#include "uvmapping.h"

#include "../constants.h"
#include "../utilities/math_utils.h"

uvmapping::uvmapping()
	: m_scale_u(1.0), m_scale_v(1.0), m_offset_u(0.0), m_offset_v(0.0), m_repeat_u(1.0), m_repeat_v(1.0)
{
}

uvmapping::uvmapping(double scale_u, double scale_v)
	: m_scale_u(scale_u), m_scale_v(scale_v), m_offset_u(0.0), m_offset_v(0.0), m_repeat_u(1.0), m_repeat_v(1.0)
{
}

uvmapping::uvmapping(double scale_u, double scale_v, double offset_u, double offset_v)
	: m_scale_u(scale_u), m_scale_v(scale_v), m_offset_u(offset_u), m_offset_v(offset_v), m_repeat_u(1.0), m_repeat_v(1.0)
{
}

uvmapping::uvmapping(double scale_u, double scale_v, double offset_u, double offset_v, double repeat_u, double repeat_v)
	: m_scale_u(scale_u), m_scale_v(scale_v), m_offset_u(offset_u), m_offset_v(offset_v), m_repeat_u(repeat_u), m_repeat_v(repeat_v)
{
}

double uvmapping::scale_u() const
{
	return m_scale_u;
}

double uvmapping::scale_v() const
{
	return m_scale_v;
}

double uvmapping::offset_u() const
{
	return m_offset_u;
}

double uvmapping::offset_v() const
{
	return m_offset_v;
}

double uvmapping::repeat_u() const
{
	return m_repeat_u;
}

double uvmapping::repeat_v() const
{
	return m_repeat_v;
}


void uvmapping::scale_u(double su)
{
	m_scale_u = su;
}

void uvmapping::scale_v(double sv)
{
	m_scale_v = sv;
}

void uvmapping::offset_u(double ou)
{
	m_offset_u = ou;
}

void uvmapping::offset_v(double ov)
{
	m_offset_v = ov;
}

void uvmapping::repeat_u(double ru)
{
	m_repeat_u = ru;
}

void uvmapping::repeat_v(double rv)
{
	m_repeat_v = rv;
}


void get_spherical_uv(const point3& p, double& u, double& v)
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

vector3 from_spherical_uv(double u, double v)
{
	double phi = 2 * M_PI * u, theta = M_PI * v;
	// THIS IS SUPER WEIRD?? Used only (AND KEEP IT THAT WAY) for environment importance sampling
	phi -= M_PI;

	return vector3(cos(phi) * sin(theta), -cos(theta), -sin(phi) * sin(theta));
}



void get_sphere_uv(const point3& p, double& u, double& v, const uvmapping& mapping)
{
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

	// Calculate spherical coordinates theta and phi
	auto theta = acos(-p.y); // Angle from the positive y-axis
	auto phi = atan2(-p.z, p.x) + M_PI; // Angle in the xy-plane around the z-axis

	// Normalize theta and phi to [0, 1] for texture coordinates
	double s = phi / (2 * M_PI); // Normalize phi to [0, 1] (u-coordinate)
	double t = theta / M_PI;     // Normalize theta to [0, 1] (v-coordinate)

	// Apply texture repetition (tiling/repeating) to s and t
	s = fmod(s * mapping.repeat_u(), 1.0); // Apply tiling to s (u-axis)
	t = fmod(t * mapping.repeat_v(), 1.0); // Apply tiling to t (v-axis)

	// Map normalized coordinates (s, t) to (u, v) texture space
	u = mapping.scale_u() * s + mapping.offset_u();
	v = mapping.scale_v() * t + mapping.offset_v();
}

void get_torus_uv(const vector3& p, vector3& c, double& u, double& v, double majorRadius, double minorRadius, const uvmapping& mapping)
{
	double phi = atan2(p.y, p.x);
	if (phi < 0) phi += 2 * M_PI; // Ensure phi is in [0, 2*pi]

	// Calculate the distance from the center of the torus in the xy-plane
	double dxy = glm::length(vector2(p.x, p.y) - vector2(c.x, c.y)) - majorRadius;

	// Calculate the angle around the torus
	double theta = atan2(p.z, dxy);
	if (theta < 0) theta += 2 * M_PI; // Ensure theta is in [0, 2*pi]

	// Map phi and theta to the range [0, 1] for u and v coordinates
	double s = phi / (2 * M_PI);
	double t = theta / (2 * M_PI);

	// Apply texture repetition (tiling/repeating) to s and t
	s = fmod(s * mapping.repeat_u(), 1.0); // Apply tiling to s (u-axis)
	t = fmod(t * mapping.repeat_v(), 1.0); // Apply tiling to t (v-axis)

	// Map normalized coordinates (s, t) to (u, v) texture space
	u = mapping.scale_u() * s + mapping.offset_u();
	v = mapping.scale_v() * t + mapping.offset_v();
}

void get_cylinder_uv(const vector3& p, double& u, double& v, double radius, double height, const uvmapping& mapping)
{
	// Calculate the angle around the cylinder using atan2
	double theta = std::atan2(p.x, p.z);

	// Map the angle (theta) to the range [0, 1] for u coordinate (s)
	double s = 1.0 - (theta + M_PI) / (2.0 * M_PI); // Invert theta and map to [0, 1]

	// Calculate the vertical height (y-coordinate) relative to the cylinder's height
	double y = p.y;
	double t = (y + height / 2.0) / height; // Map y-coordinate to [0, 1] range

	// Apply texture repetition (tiling/repeating) to s and t
	s = fmod(s * mapping.repeat_u(), 1.0); // Apply tiling to s (u-axis)
	t = fmod(t * mapping.repeat_v(), 1.0); // Apply tiling to t (v-axis)

	// Map normalized coordinates (s, t) to (u, v) texture space
	u = mapping.scale_u() * s + mapping.offset_u();
	v = mapping.scale_v() * t + mapping.offset_v();
}

void get_disk_uv(const vector3& p, double& u, double& v, double radius, const uvmapping& mapping)
{
	//// Calculate the angle around the disk using atan2
	//double theta = std::atan2(p.x, p.z);

	//// Map the angle (theta) to the range [0, 1] for u coordinate (s)
	//double s = 1.0 - (theta + M_PI) / (2.0 * M_PI); // Invert theta and map to [0, 1]

	//// Calculate the vertical height (phi) relative to the disk's radius
	//double phi = std::atan2(p.y, radius);

	//// Map the vertical height (phi) to the range [0, 1] for v coordinate (t)
	//double t = (phi + M_PI / 2.0) / M_PI; // Map phi to [0, 1] range

	//// Apply texture repetition (tiling/repeating) to s and t
	//s = fmod(s * mapping.repeat_u(), 1.0); // Apply tiling to s (u-axis)
	//t = fmod(t * mapping.repeat_v(), 1.0); // Apply tiling to t (v-axis)

	//// Map normalized coordinates (s, t) to (u, v) texture space
	//u = mapping.scale_u() * s + mapping.offset_u();
	//v = mapping.scale_v() * t + mapping.offset_v();


	// Ensure point p is within the disk's radius
	double x = p.x;
	double z = p.z;

	// Map x and z coordinates to the range [0, 1] based on the disk's radius
	double s = (x / (2.0 * radius)) + 0.5;
	double t = (z / (2.0 * radius)) + 0.5;

	// Apply texture repetition (tiling/repeating) to s and t
	s = fmod(s * mapping.repeat_u(), 1.0); // Apply tiling to s (u-axis)
	t = fmod(t * mapping.repeat_v(), 1.0); // Apply tiling to t (v-axis)

	// Map normalized coordinates (s, t) to (u, v) texture space
	u = mapping.scale_u() * s + mapping.offset_u();
	v = mapping.scale_v() * t + mapping.offset_v();
}

void get_cone_uv(const vector3& p, double& u, double& v, double radius, double height, const uvmapping& mapping)
{
	// Calculate the angle around the cone using atan2
	double theta = atan2(p.x, p.z);

	// Map the angle (theta) to the range [0, 1] for u coordinate
	double s = (theta + M_PI) / (2 * M_PI);

	// Calculate the distance from the cone apex to the point
	double distance = sqrt(p.x * p.x + p.z * p.z);

	// Map the distance to the range [0, 1] for v coordinate
	double t = distance / radius; // Normalize distance by radius

	// Apply texture repetition (tiling/repeating) to s and t
	s = fmod(s * mapping.repeat_u(), 1.0); // Apply tiling to s (u-axis)
	t = fmod(t * mapping.repeat_v(), 1.0); // Apply tiling to t (v-axis)

	// Map normalized coordinates (s, t) to (u, v) texture space
	u = mapping.scale_u() * s + mapping.offset_u();
	v = mapping.scale_v() * t + mapping.offset_v();
}

void get_xy_rect_uv(double x, double y, double& u, double& v, float x0, float x1, float y0, float y1, const uvmapping& mapping)
{
	// Calculate normalized coordinates (s, t) within the range [0, 1]
	double s = (x - x0) / (x1 - x0);
	double t = (y - y0) / (y1 - y0);

	// Apply tiling to the normalized coordinates
	s = fmod(s * mapping.repeat_u(), 1.0); // Apply tiling to s (u-axis)
	t = fmod(t * mapping.repeat_v(), 1.0); // Apply tiling to t (v-axis)

	// Map normalized coordinates (s, t) to (u, v) texture space
	u = mapping.scale_u() * s + mapping.offset_u();
	v = mapping.scale_v() * t + mapping.offset_v();
}

void get_xz_rect_uv(double x, double z, double& u, double& v, float x0, float x1, float z0, float z1, const uvmapping& mapping)
{
	// Calculate normalized coordinates (s, t) within the range [0, 1]
	double s = (x - x0) / (x1 - x0);
	double t = (z - z0) / (z1 - z0);

	// Apply tiling to the normalized coordinates
	s = fmod(s * mapping.repeat_u(), 1.0); // Apply tiling to s (u-axis)
	t = fmod(t * mapping.repeat_v(), 1.0); // Apply tiling to t (v-axis)

	// Map normalized coordinates (s, t) to (u, v) texture space
	u = mapping.scale_u() * s + mapping.offset_u();
	v = mapping.scale_v() * t + mapping.offset_v();
}

void get_yz_rect_uv(double y, double z, double& u, double& v, float y0, float y1, float z0, float z1, const uvmapping& mapping)
{
	// Calculate normalized coordinates (s, t) within the range [0, 1]
	double s = (y - y0) / (y1 - y0);
	double t = (z - z0) / (z1 - z0);

	// Apply tiling to the normalized coordinates
	s = fmod(s * mapping.repeat_u(), 1.0); // Apply tiling to s (u-axis)
	t = fmod(t * mapping.repeat_v(), 1.0); // Apply tiling to t (v-axis)

	// Map normalized coordinates (s, t) to (u, v) texture space
	u = mapping.scale_u() * s + mapping.offset_u();
	v = mapping.scale_v() * t + mapping.offset_v();
}

void get_triangle_uv(const vector3 hitpoint, double& u, double& v, const vector3 verts[3], const vector2 vert_uvs[3])
{
	// https://www.irisa.fr/prive/kadi/Cours_LR2V/Cours/RayTracing_Texturing.pdf
	// https://computergraphics.stackexchange.com/questions/7738/how-to-assign-calculate-triangle-texture-coordinates
	
	double u1 = 0.0, v1 = 0.0, w1 = 0.0;
	// get triangle hit point barycenter uvs coords
	get_barycenter(hitpoint, verts[0], verts[1], verts[2], u1, v1, w1);

	// translate uvs from 3d space to texture space
	vector2 uv = calculateTextureCoordinate(vert_uvs[0], vert_uvs[1], vert_uvs[2], vector2(u1, v1));

	u = uv.x;
	v = uv.y;
}


/// <summary>
/// Function to calculate texture coordinates using barycentric coordinates
/// </summary>
vector2 calculateTextureCoordinate(vector2 uv0, vector2 uv1, vector2 uv2, const vector2& barycentricCoords)
{
	double u = (barycentricCoords.x * uv0.x + barycentricCoords.y * uv1.x + (1.0f - barycentricCoords.x - barycentricCoords.y) * uv2.x);
	double v = (barycentricCoords.x * uv0.y + barycentricCoords.y * uv1.y + (1.0f - barycentricCoords.x - barycentricCoords.y) * uv2.y);

	// Apply texture repeat (wrap) behavior
	u = std::fmod(u, 1.0);
	v = std::fmod(v, 1.0);
	if (u < 0.0) u += 1.0;
	if (v < 0.0) v += 1.0;

	return vector2(u, v); // Return texture coordinates
}

/// <summary>
/// TODO ! Could be enhanced by using stb_resize probably !
/// </summary>
void get_screen_uv(int x, int y, double texture_width, double texture_height, double render_width, double render_height, double& u, double& v)
{
	// Calculate normalized coordinates (u, v) within the range [0, 1]
	// Normalize pixel coordinates to [0, 1] with proper floating-point division
	u = (x / texture_width) * (texture_width / render_width);
	v = 1.0 - ((y / texture_height) * (texture_height / render_height));
}