#include "spot_light.h"

#include "../constants.h"
#include "../materials/diffuse_spot_light.h"
#include "../textures/solid_color_texture.h"
#include "../utilities/math_utils.h"
#include "../utilities/randomizer.h"
#include "../utilities/uvmapping.h"
#include "../misc/singleton.h"
#include "../onb.h"

spot_light::spot_light(vector3 _direction, double _cosTotalWidth, double _cosFalloffStart, double _intensity, color _color, string _name, bool _invisible)
    : light(_direction, _intensity, _color, _invisible, _name)
{
    m_cosTotalWidth = _cosTotalWidth;
    m_cosFalloffStart = _cosFalloffStart;

    m_mat = std::make_shared<diffuse_spot_light>(std::make_shared<solid_color_texture>(_color), _direction, _cosTotalWidth, _cosFalloffStart, _intensity, m_invisible);




    double radius = 1.0;
    double height = 5.0;

    // calculate cone bounding box for ray optimizations
    m_bbox = aabb(
        vector3(_direction.x - radius, _direction.y, _direction.z - radius),
        vector3(_direction.x + radius, _direction.y + height, _direction.z + radius)
    );
}

aabb spot_light::bounding_box() const
{
    return m_bbox;
}

bool spot_light::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    double A = r.origin().x - m_direction.x;
    double B = r.origin().z - m_direction.z;
    double D = m_cone_height - r.origin().y + m_direction.y;

    double tan = (m_cone_radius / m_cone_height) * (m_cone_radius / m_cone_height);

    double a = (r.direction().x * r.direction().x) + (r.direction().z * r.direction().z) - (tan * (r.direction().y * r.direction().y));
    double b = (2 * A * r.direction().x) + (2 * B * r.direction().z) + (2 * tan * D * r.direction().y);
    double c = (A * A) + (B * B) - (tan * (D * D));

    double delta = b * b - 4 * (a * c);
    if (fabs(delta) < 0.001) return false;

    if (delta < 0)
    {
        return false;
    }

    double root = (-b - sqrt(delta)) / (2 * a);

    if (root < ray_t.min || ray_t.max < root)
    {
        root = (-b + sqrt(delta)) / (2 * a);
        if (root < ray_t.min || ray_t.max < root)
        {
            return false;
        }
    }


    double y = r.origin().y + root * r.direction()[1];

    if ((y < m_direction.y) || (y > m_direction.y + m_cone_height))
    {
        return false;
    }

    //double rs = sqrt((rec.hit_point.x - center.x) * (rec.hit_point.x - center.x) + (rec.hit_point.z - center.z) * (rec.hit_point.z - center.z));
    //vector3 n = vector3(rec.hit_point.x - center.x, rs * (radius / height), rec.hit_point.z - center.z);

    Singleton* singleton = Singleton::getInstance();
    if (singleton)
    {
        auto renderParams = singleton->value();
        if (m_invisible && depth == renderParams.recursionMaxDepth)
        {
            return false;
        }
    }


    rec.t = root;
    rec.hit_point = r.at(rec.t);
    //rec.normal = glm::normalize(n);
    /*vector3 outward_normal = (rec.hit_point - center) / radius;
    rec.set_face_normal(r, outward_normal);*/


    // Calculate the outward normal
    vector3 outward_normal;
    if (rec.hit_point.y <= m_direction.y) {
        // Point lies on the base of the cone
        outward_normal = vector3(0, -1, 0);
    }
    else if (rec.hit_point.y >= m_direction.y + m_cone_height) {
        // Point lies on the top of the cone
        outward_normal = vector3(0, 1, 0);
    }
    else {
        // Point lies on the curved surface of the cone
        double rs = sqrt((rec.hit_point.x - m_direction.x) * (rec.hit_point.x - m_direction.x) + (rec.hit_point.z - m_direction.z) * (rec.hit_point.z - m_direction.z));
        outward_normal = vector3(rec.hit_point.x - m_direction.x, rs * (m_cone_radius / m_cone_height), rec.hit_point.z - m_direction.z);
    }

    // Normalize the outward normal
    //rec.normal = glm::normalize(outward_normal);

    rec.set_face_normal(r, glm::normalize(outward_normal));



    get_cone_uv(outward_normal, rec.u, rec.v, m_cone_radius, m_cone_height, m_mapping);
    rec.mat = m_mat;
    rec.name = m_name;
    rec.bbox = m_bbox;

    return true;
}

double spot_light::pdf_value(const point3& origin, const vector3& v) const
{
    hit_record rec;
    ray r(origin, v);
    if (!this->hit(r, interval(0.001, infinity), rec, 0)) {
        return 0;
    }

    double cos_theta_max = sqrt(1 - (m_cone_radius * m_cone_radius) / ((m_cone_radius * m_cone_radius) + (m_cone_height * m_cone_height)));
    double solid_angle = 2 * M_PI * (1 - cos_theta_max);
    return 1 / solid_angle;
}

/// <summary>
/// Random special implementation for quad light (override base)
/// </summary>
/// <param name="origin"></param>
/// <returns></returns>
vector3 spot_light::random(const point3& origin) const
{
    // Generate a random point on the cone surface
    double theta = 2 * M_PI * randomizer::random_double();
    double z = randomizer::random_double();
    double r = m_cone_radius * (1 - z);

    point3 random_point = m_direction + vector3(r * cos(theta), z * m_cone_height, r * sin(theta));
    return glm::normalize(random_point - origin);
}