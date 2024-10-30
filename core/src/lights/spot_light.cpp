#include "spot_light.h"


#include "../constants.h"
#include "../materials/diffuse_light.h"
#include "../materials/diffuse_spot_light.h"
#include "../textures/solid_color_texture.h"
#include "../utilities/math_utils.h"
#include "../utilities/uvmapping.h"
#include "../misc/singleton.h"
#include "../misc/onb.h"


spot_light::spot_light(point3 position, vector3 direction, double cutoff, double falloff, double intensity, double radius, color rgb, string name, bool invisible)
    : light(position, intensity, rgb, invisible, name)
{
	m_direction = direction;
	m_radius = radius;
	m_cutoff = cos(degrees_to_radians(cutoff));
	m_falloff = falloff;

    m_mat = std::make_shared<diffuse_spot_light>(std::make_shared<solid_color_texture>(m_color), m_position, m_direction, m_cutoff, m_falloff, m_intensity, m_invisible);

	// calculate stationary sphere bounding box for ray optimizations
	vector3 rvec = vector3(m_radius);
	m_bbox = aabb(m_position - rvec, m_position + rvec);
}

aabb spot_light::bounding_box() const
{
    return m_bbox;
}

bool spot_light::hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const
{
	point3 center = m_position;
	vector3 oc = r.origin() - center;
	auto a = vector_length_squared(r.direction());
	auto half_b = glm::dot(oc, r.direction());
	auto c = vector_length_squared(oc) - m_radius * m_radius;

	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	auto sqrtd = sqrt(discriminant);

	// Find the nearest root that lies in the acceptable range.
	auto root = (-half_b - sqrtd) / a;
	if (!ray_t.surrounds(root)) {
		root = (-half_b + sqrtd) / a;
		if (!ray_t.surrounds(root))
			return false;
	}

	Singleton* singleton = Singleton::getInstance();
	if (singleton)
	{
		auto renderParams = singleton->value();
		if (m_invisible && depth == renderParams.recursionMaxDepth)
		{
			return false;
		}
	}


	// number of hits encountered by the ray (only the nearest ?)
	rec.t = root;

	// point coordinate of the hit
	rec.hit_point = r.at(rec.t);

	// material of the hit object
	rec.mat = m_mat;

	// name of the primitive hit by the ray
	rec.name = m_name;
	rec.bbox = m_bbox;

	// set normal and front-face tracking
	vector3 outward_normal = (rec.hit_point - center) / m_radius;
	rec.set_face_normal(r, outward_normal);

	// UV coordinates
	const uvmapping mapping = uvmapping();
	get_sphere_uv(outward_normal, rec.u, rec.v, mapping);

	return true;
}

double spot_light::pdf_value(const point3& o, const vector3& v, randomizer& rnd) const
{
	// This method only works for stationary spheres.
	hit_record rec;
	if (!this->hit(ray(o, v), interval(SHADOW_ACNE_FIX, infinity), rec, 0, rnd))
		return 0;

	auto cos_theta_max = sqrt(1 - m_radius * m_radius / vector_length_squared(m_position - o));
	auto solid_angle = 2 * M_PI * (1 - cos_theta_max);

	return  1 / solid_angle;
}

vector3 spot_light::random(const point3& o, randomizer& rnd) const
{
	vector3 direction = m_position - o;
	auto distance_squared = vector_length_squared(direction);
	onb uvw;
	uvw.build_from_w(direction);
	return uvw.local(rnd.random_to_sphere(m_radius, distance_squared));
}