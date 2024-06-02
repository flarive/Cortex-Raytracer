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


#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


spot_light2::spot_light2(point3 position, vector3 direction, double cutoff, double falloff, double intensity, double radius, double blur, color rgb, std::string name, bool invisible)
    : light(position, intensity, rgb, invisible, name)
{
    m_direction = direction;
    m_radius = radius;
    m_cutoff = cos(degrees_to_radians(cutoff));
    m_falloff = falloff;
    m_blur = blur;

    m_mat = std::make_shared<diffuse_spot_light>(std::make_shared<solid_color_texture>(m_color), m_position, m_direction, m_cutoff, m_falloff, m_intensity, m_invisible);

    // rotate disk diffuse emissive primitive according to spot light direction
    rotate_disk();
}


bool spot_light2::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    vector4 local_ray_origin = m_worldToLocal * vector4(r.origin(), 1.0f);
    vector4 local_ray_direction = m_worldToLocal * vector4(r.direction(), 0.0f);

    ray local_ray(point3(local_ray_origin.x, local_ray_origin.y, local_ray_origin.z), vector3(local_ray_direction));

    // Compute the intersection with the plane containing the disk
    double t = -local_ray.origin().y / local_ray.direction().y;
    if (t < ray_t.min || t > ray_t.max)
        return false;

    point3 local_hit_point = local_ray.at(t);

    // Check if the hit point is within the disk's radius
    double dist_squared = local_hit_point.x * local_hit_point.x + local_hit_point.z * local_hit_point.z;
    if (dist_squared > m_radius * m_radius)
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
    rec.hit_point = vector3(m_localToWorld * vector4(local_hit_point, 1.0f));
    rec.mat = m_mat;
    rec.name = m_name;
    rec.bbox = m_bbox;

    vector3 local_normal = glm::normalize(vector3(local_hit_point.x, 0, local_hit_point.z));
    vector3 world_normal = glm::normalize(vector3(m_localToWorld * vector4(local_normal, 0.0f)));

    // Ensure outward_normal points outward (opposite to ray direction)
    if (dot(world_normal, r.direction()) > 0)
        world_normal = -world_normal;

    rec.set_face_normal(r, world_normal);

    // Compute UV coordinates
    get_disk_uv(local_normal, rec.u, rec.v, m_radius, m_mapping);

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

    point3 local_random_point(r * cos(theta), 0, r * sin(theta));
    point3 world_random_point = vector3(m_localToWorld * vector4(local_random_point, 1.0f));

    return world_random_point - o;
}

aabb spot_light2::bounding_box() const
{
    return m_bbox;
}

void spot_light2::rotate_disk()
{
    vector3 default_direction(0.0f, -1.0f, 0.0f); // Assuming the disk is facing downward by default
    m_rotation = glm::quatLookAt(glm::normalize(m_direction), vector3(0.0f, 1.0f, 0.0f));

    // Define the fixed rotation offset quaternion (for example, rotate 45 degrees around the Z-axis)
    glm::quat fixed_rotation = glm::angleAxis(degrees_to_radians(90.0), vector3(0.0f, 0.0f, 1.0f));

    // Combine the spotlight direction rotation with the fixed rotation offset
    m_rotation = fixed_rotation * m_rotation;

    matrix4 rotation = glm::toMat4(m_rotation);
    matrix4 translation = glm::translate(matrix4(1.0f), vector3(m_position));
    m_localToWorld = translation * rotation;
    m_worldToLocal = glm::inverse(m_localToWorld);

    // Calculate the bounding box after the transformation
    vector3 min_point = vector3(m_localToWorld * vector4(-m_radius, -m_height / 2, -m_radius, 1.0f));
    vector3 max_point = vector3(m_localToWorld * vector4(m_radius, m_height / 2, m_radius, 1.0f));

    m_bbox = aabb(
        point3(std::min(min_point.x, max_point.x), std::min(min_point.y, max_point.y), std::min(min_point.z, max_point.z)),
        point3(std::max(min_point.x, max_point.x), std::max(min_point.y, max_point.y), std::max(min_point.z, max_point.z))
    );
}
