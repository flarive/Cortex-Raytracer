#include "spot_light2.h"

#include "../constants.h"
#include "../materials/diffuse_light.h"
#include "../materials/diffuse_spot_light.h"
#include "../textures/solid_color_texture.h"
#include "../utilities/math_utils.h"
#include "../utilities/randomizer.h"
#include "../utilities/uvmapping.h"
#include "../misc/singleton.h"
#include "../onb.h"


spot_light2::spot_light2(point3 position, vector3 direction, double cutoff, double falloff, double intensity, double radius, double blur, color rgb, std::string name, bool invisible)
    : light(position, intensity, rgb, invisible, name)
{
    m_direction = direction;
    m_radius = radius;
    m_cutoff = cos(degrees_to_radians(cutoff));
    m_falloff = falloff;
    m_blur = blur;

    m_mat = std::make_shared<diffuse_spot_light>(std::make_shared<solid_color_texture>(m_color), m_position, m_direction, m_cutoff, m_falloff, m_blur, m_intensity, m_invisible);

    // calculate disk bounding box for ray optimizations
    m_bbox = aabb(position - vector3(radius, m_height / 2, radius), position + vector3(radius, m_height / 2, radius));
}


bool spot_light2::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    // Compute the intersection with the plane containing the disk
    double t = (m_position.y - r.origin().y) / r.direction().y;
    if (t < ray_t.min || t > ray_t.max)
        return false;

    point3 hit_point = r.at(t);

    // Check if the hit point is within the disk's radius
    double dist_squared = (hit_point.x - m_position.x) * (hit_point.x - m_position.x) + (hit_point.z - m_position.z) * (hit_point.z - m_position.z);
    if (dist_squared > m_radius * m_radius)
        return false;

    // Check if the hit point is within the disk's height
    if (hit_point.y < m_position.y - m_height / 2 || hit_point.y > m_position.y + m_height / 2)
        return false;

    Singleton* singleton = Singleton::getInstance();
    if (singleton)
    {
        auto renderParams = singleton->value();
        if (m_invisible && depth == renderParams.recursionMaxDepth)
        {
            return false;
        }
    }

    rec.t = t;
    rec.hit_point = r.at(rec.t);

    rec.mat = m_mat;
    rec.name = m_name;
    rec.bbox = m_bbox;


    //rec.normal = vector3((rec.hit_point.x - m_position.x) / m_radius, 0, (rec.hit_point.z - m_position.z) / m_radius);
    //vector3 outward_normal = (rec.hit_point - m_position) / m_radius;
    vector3 outward_normal = glm::normalize((rec.hit_point - m_position) / m_radius);

    // Ensure outward_normal points outward (opposite to ray direction)
    if (dot(outward_normal, r.direction()) > 0)
        outward_normal = -outward_normal;

    rec.set_face_normal(r, outward_normal);

    // compute UV coordinates
    get_disk_uv(outward_normal, rec.u, rec.v, m_radius, m_mapping);
    

    return true;
}

double spot_light2::pdf_value(const point3& o, const vector3& v) const
{
    hit_record rec;
    ray r(o, v);
    if (!this->hit(r, interval(0.001, std::numeric_limits<double>::infinity()), rec, 0))
        return 0;

    double area = M_PI * m_radius * m_radius;
    double distance_squared = rec.t * rec.t * glm::length(v);
    double cosine = fabs(glm::dot(v, rec.normal) / glm::length(v));

    return distance_squared / (cosine * area);
}

vector3 spot_light2::random(const point3& o) const
{
    // Generate a random point on the disk
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    double r = m_radius * sqrt(dis(gen));
    double theta = 2 * M_PI * dis(gen);

    point3 random_point(
        m_position.x + r * cos(theta),
        m_position.y,
        m_position.z + r * sin(theta)
    );

    return random_point - o;
}

aabb spot_light2::bounding_box() const
{
    return m_bbox;
}