#include "directional_light.h"

#include "../constants.h"
#include "../misc/singleton.h"
#include "../materials/diffuse_light.h"
#include "../utilities/math_utils.h"

#include <glm/glm.hpp>

directional_light::directional_light(const point3& _position, const vector3& _u, const vector3& _v, double _intensity, color _color, std::string _name, bool _invisible)
    : light(_position, _intensity, _color, _invisible, _name), m_u(_u), m_v(_v)
{
    m_mat = std::make_shared<diffuse_light>(m_color, _intensity, true, m_invisible);

    auto n = glm::cross(m_u, m_v);
    m_normal = unit_vector(n);
    D = glm::dot(m_normal, m_position);
    w = n / glm::dot(n, n);

    area = vector_length(n);

    set_bounding_box();
}
    
void directional_light::set_bounding_box()
{
    //m_bbox = aabb(m_position, m_position + m_u + m_v).pad();

    // Calculate the quad's corners
    point3 corner1 = m_position - 0.5 * (m_u + m_v);
    point3 corner2 = m_position + 0.5 * (m_u + m_v);

    m_bbox = aabb(corner1, corner2).pad();
}

aabb directional_light::bounding_box() const
{
    return m_bbox;
}

bool directional_light::hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const
{
    auto denom = glm::dot(m_normal, r.direction());

    // No hit if the ray is parallel to the plane.
    if (fabs(denom) < 1e-8)
    {
        return false;
    }

    // Return false if the hit point parameter t is outside the ray interval.
    double t = (D - glm::dot(m_normal, r.origin())) / denom;
    if (!ray_t.contains(t))
    {
        return false;
    }

    // Determine the hit point lies within the planar shape using its plane coordinates.
    auto intersection = r.at(t);
    vector3 planar_hitpt_vector = intersection - m_position;
    auto alpha = glm::dot(w, glm::cross(planar_hitpt_vector, m_v));
    auto beta = glm::dot(w, glm::cross(m_u, planar_hitpt_vector));

    if (!is_interior(alpha, beta, rec))
    {
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

    // Ray hits the 2D shape; set the rest of the hit record and return true.
    rec.t = t;
    rec.hit_point = intersection;
    rec.mat = m_mat;
    rec.name = m_name;
    rec.bbox = m_bbox;
    rec.set_face_normal(r, m_normal);

    return true;
}

bool directional_light::is_interior(double a, double b, hit_record& rec) const
{
    // Given the hit point in plane coordinates, return false if it is outside the
    // primitive, otherwise set the hit record UV coordinates and return true.

    //if ((a < 0) || (1 < a) || (b < 0) || (1 < b))
    //{
    //    return false;
    //}

    //rec.u = a;
    //rec.v = b;
    //return true;

    if ((a < -0.5) || (0.5 < a) || (b < -0.5) || (0.5 < b))
    {
        return false;
    }

    rec.u = a + 0.5; // shift to [0, 1] range
    rec.v = b + 0.5; // shift to [0, 1] range
    return true;
}

double directional_light::pdf_value(const point3& origin, const vector3& v, randomizer& rnd) const
{
    hit_record rec;

    if (!this->hit(ray(origin, v), interval(SHADOW_ACNE_FIX, infinity), rec, 0, rnd))
        return 0;

    auto distance_squared = rec.t * rec.t * vector_length_squared(v);
    auto cosine = fabs(dot(v, rec.normal) / vector_length(v));

    return distance_squared / (cosine * area);
}

/// <summary>
/// Random special implementation for quad light (override base)
/// </summary>
/// <param name="origin"></param>
/// <returns></returns>
vector3 directional_light::random(const point3& origin, randomizer& rnd) const
{
    auto p = m_position 
        + (rnd.get_real(0.0, 1.0) - 0.5) * m_u 
        + (rnd.get_real(0.0, 1.0) - 0.5) * m_v;

    return p - origin;
}